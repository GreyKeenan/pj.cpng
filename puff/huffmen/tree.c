#ifdef DEBUG
#include <stdio.h>
#endif

#include "./tree.h"
#include "./tree_impl.h"

#include <stddef.h>
#include <math.h>

#define IMPOSSIBLE Puff_Tree_IMPOSSIBLE


static inline int mike_decompress_huffmen_tree_getNode32(const struct Puff_Tree *self, uint16_t nodeIndex, uint32_t *destination);
static inline int mike_decompress_huffmen_tree_setNode32(struct Puff_Tree *self, uint16_t nodeIndex, uint32_t node32);

static inline uint16_t mike_decompress_huffmen_tree_Node32_shiftOut(uint32_t node, _Bool lr, uint8_t childBitLength);
/*
	returns the value & type of lr child in the form:
		[0 high-bits][childBitLength bits value][1-bit type]
*/
static inline void mike_decompress_huffmen_tree_Node32_shiftIn(uint32_t *node, _Bool lr, uint16_t value, _Bool type, uint8_t childBitLength);
/*
	sets the lr child of node to the value & type
*/


int Puff_Tree_init(struct Puff_Tree *self, uint8_t *data, uint16_t cap, uint16_t maxLeaves) {
	if (data == NULL) {
		return 1;
	}
	if (maxLeaves < 2) {
		return 2;
	}

	uint16_t maxNodes = maxLeaves - 1;

	uint8_t childBitLength = ceil(log2(maxLeaves));
	uint8_t nodeBytes = ceil(((childBitLength + 1) * 2) /(float) 8);
	
	if (nodeBytes > 4) {
		return 3;
	}
	if ((uint32_t)maxNodes * nodeBytes > cap) {
		return 4;
	}


	*self = (struct Puff_Tree) {
		.data = data,
		.maxNodeCount = maxNodes,
		.nodeCount = 1,
		.nodeBytes = nodeBytes,
		.childBitLength = childBitLength
	};
	if (mike_decompress_huffmen_tree_setNode32(self, Puff_Tree_ROOT, 0)) {
		return IMPOSSIBLE;
	}
	return 0;
}


// main functionality
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int Puff_Tree_walk(const struct Puff_Tree *self, uint16_t parentIndex, _Bool lr, uint16_t *destination) {
	int e = 0;
	uint32_t parent32 = 0;
	e = mike_decompress_huffmen_tree_getNode32(self, parentIndex, &parent32);
	if (e) return e;

	uint16_t child = mike_decompress_huffmen_tree_Node32_shiftOut(parent32, lr, self->childBitLength);
	if (!child) {
		return Puff_Tree_HALT;
	}

	_Bool type_isLeaf = child & 1;
	*destination = child >> 1;
	return type_isLeaf ? Puff_Tree_ISLEAF : Puff_Tree_ISNODE;
}

int Puff_Tree_birthNode(struct Puff_Tree *self, uint16_t parentIndex, _Bool lr, uint16_t *nNewborn) {

	if (self->nodeCount >= self->maxNodeCount) {
		return Puff_Tree_TOOMANYKIDS;
	}

	int e = 0;
	uint32_t parent32 = 0;
	e = mike_decompress_huffmen_tree_getNode32(self, parentIndex, &parent32);
	if (e) return e;

	uint16_t child = mike_decompress_huffmen_tree_Node32_shiftOut(parent32, lr, self->childBitLength);
	if (child) {
		return Puff_Tree_COLLISION;
	}

	uint16_t newbornIndex = self->nodeCount;
	self->nodeCount++;

	e = mike_decompress_huffmen_tree_setNode32(self, newbornIndex, 0);
	if (e) return IMPOSSIBLE;
	
	mike_decompress_huffmen_tree_Node32_shiftIn(&parent32, lr, newbornIndex, Puff_Tree_TYPE_NODE, self->childBitLength);
	e = mike_decompress_huffmen_tree_setNode32(self, parentIndex, parent32);
	if (e) return IMPOSSIBLE;

	if (nNewborn != NULL) {
		*nNewborn = newbornIndex;
	}
	return 0;
}
int Puff_Tree_growLeaf(struct Puff_Tree *self, uint16_t parentIndex, _Bool lr, uint16_t value) {

	if (value >> self->childBitLength) {
		return Puff_Tree_BADVALUE;
	}

	int e = 0;
	uint32_t parent32 = 0;
	e = mike_decompress_huffmen_tree_getNode32(self, parentIndex, &parent32);
	if (e) return e;

	uint16_t child = mike_decompress_huffmen_tree_Node32_shiftOut(parent32, lr, self->childBitLength);
	if (child) {
		return Puff_Tree_COLLISION;
	}

	mike_decompress_huffmen_tree_Node32_shiftIn(&parent32, lr, value, Puff_Tree_TYPE_LEAF, self->childBitLength);
	e = mike_decompress_huffmen_tree_setNode32(self, parentIndex, parent32);
	if (e) return e;

	return 0;
}

int Puff_Tree_enterCode(struct Puff_Tree *self, uint16_t code, uint16_t codeLength, uint16_t value) {

	#ifdef DEBUG
	printf("entering code: 0x%x(%d) : %d\n", code, codeLength, value);
	#endif

	int e = 0;

	uint16_t nodeIndex = Puff_Tree_ROOT;
	uint16_t child = 0;

	for (int i = 1 << (codeLength - 1); i != 0; i >>= 1) {
		if (i == 1) {
			e = Puff_Tree_growLeaf(self, nodeIndex, code & i, value);
			if (e) return e;
			continue;
		}
		e = Puff_Tree_walk(self, nodeIndex, code & i, &child);
		switch (e) {
			case Puff_Tree_ISNODE:
				nodeIndex = child;
				break;
			case Puff_Tree_HALT:
				e = Puff_Tree_birthNode(self, nodeIndex, code & i, &child);
				if (e) return e;
				nodeIndex = child;
				break;
			default:
				return e;
		}
	}

	return 0;
}

// lower
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

static inline int mike_decompress_huffmen_tree_getNode32(const struct Puff_Tree *self, uint16_t nodeIndex, uint32_t *destination) {
	if (nodeIndex >= self->nodeCount) {
		return Puff_Tree_OUTOFBOUNDS;
	}

	uint32_t n = 0;
	uint8_t *nodeData = self->data + (uint32_t)nodeIndex * self->nodeBytes;
	for (int i = 0; i < self->nodeBytes; ++i) {
		n |= (uint32_t)nodeData[i] << i * 8;
	}

	*destination = n;
	return 0;
}
static inline int mike_decompress_huffmen_tree_setNode32(struct Puff_Tree *self, uint16_t nodeIndex, uint32_t node32) {
	if (nodeIndex >= self->nodeCount) {
		return Puff_Tree_OUTOFBOUNDS;
	}

	uint8_t *nodeData = self->data + (uint32_t)nodeIndex * self->nodeBytes;
	for (int i = 0; i < self->nodeBytes; ++i) {
		nodeData[i] = node32 & 0xff;
		node32 >>= 8;
	}

	return 0;
}

static inline uint16_t mike_decompress_huffmen_tree_Node32_shiftOut(uint32_t node, _Bool lr, uint8_t childBitLength) {
	return (node >> (childBitLength + 1) * lr) & ((1 << childBitLength + 1) - 1);
}
static inline void mike_decompress_huffmen_tree_Node32_shiftIn(uint32_t *node, _Bool lr, uint16_t value, _Bool type, uint8_t childBitLength) {
	*node = (*node & ((1 << childBitLength + 1) - 1 << (childBitLength + 1) * !lr)) | (((uint32_t)value << 1 | type) << (childBitLength + 1) * lr);
	/*
		childBitLength++;

		mask = (1 << childBitLength) - 1;
		mask <<= childBitLength * !lr;
		node &= mask;

		value = value << 1 | type;
		value <<= childBitLength * lr;

		node |= value;
	*/
}
