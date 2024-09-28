
#include "./tree.h"
#include "./tree_impl.h"

#include <stddef.h>
#include <math.h>

#define IMPOSSIBLE Puff_Huffmen_Tree_IMPOSSIBLE


static inline int mike_decompress_huffmen_tree_getNode32(const struct Puff_Huffmen_Tree *self, uint16_t nodeIndex, uint32_t *destination);
static inline int mike_decompress_huffmen_tree_setNode32(struct Puff_Huffmen_Tree *self, uint16_t nodeIndex, uint32_t node32);

static inline uint16_t mike_decompress_huffmen_tree_Node32_shiftOut(uint32_t node, _Bool lr, uint8_t childBitLength);
/*
	returns the value & type of lr child in the form:
		[0 high-bits][childBitLength bits value][1-bit type]
*/
static inline void mike_decompress_huffmen_tree_Node32_shiftIn(uint32_t *node, _Bool lr, uint16_t value, _Bool type, uint8_t childBitLength);
/*
	sets the lr child of node to the value & type
*/


int Puff_Huffmen_Tree_init(struct Puff_Huffmen_Tree *self, uint8_t *data, uint16_t cap, uint16_t maxLeaves) {
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


	*self = (struct Puff_Huffmen_Tree) {
		.data = data,
		.maxNodeCount = maxNodes,
		.nodeCount = 1,
		.nodeBytes = nodeBytes,
		.childBitLength = childBitLength
	};
	if (mike_decompress_huffmen_tree_setNode32(self, Puff_Huffmen_Tree_ROOT, 0)) {
		return IMPOSSIBLE;
	}
	return 0;
}


// main functionality
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int Puff_Huffmen_Tree_walk(const struct Puff_Huffmen_Tree *self, uint16_t parentIndex, _Bool lr, uint16_t *destination) {
	int e = 0;
	uint32_t parent32 = 0;
	e = mike_decompress_huffmen_tree_getNode32(self, parentIndex, &parent32);
	if (e) return e;

	uint16_t child = mike_decompress_huffmen_tree_Node32_shiftOut(parent32, lr, self->childBitLength);
	if (!child) {
		return Puff_Huffmen_Tree_HALT;
	}

	_Bool type_isLeaf = child & 1;
	*destination = child >> 1;
	return type_isLeaf ? Puff_Huffmen_Tree_ISLEAF : Puff_Huffmen_Tree_ISNODE;
}

int Puff_Huffmen_Tree_birthNode(struct Puff_Huffmen_Tree *self, uint16_t parentIndex, _Bool lr, uint16_t *nNewborn) {

	if (self->nodeCount >= self->maxNodeCount) {
		return Puff_Huffmen_Tree_TOOMANYKIDS;
	}

	int e = 0;
	uint32_t parent32 = 0;
	e = mike_decompress_huffmen_tree_getNode32(self, parentIndex, &parent32);
	if (e) return e;

	uint16_t child = mike_decompress_huffmen_tree_Node32_shiftOut(parent32, lr, self->childBitLength);
	if (child) {
		return Puff_Huffmen_Tree_COLLISION;
	}

	uint16_t newbornIndex = self->nodeCount;
	self->nodeCount++;

	e = mike_decompress_huffmen_tree_setNode32(self, newbornIndex, 0);
	if (e) return IMPOSSIBLE;
	
	mike_decompress_huffmen_tree_Node32_shiftIn(&parent32, lr, newbornIndex, Puff_Huffmen_Tree_TYPE_NODE, self->childBitLength);
	e = mike_decompress_huffmen_tree_setNode32(self, parentIndex, parent32);
	if (e) return IMPOSSIBLE;

	if (nNewborn != NULL) {
		*nNewborn = newbornIndex;
	}
	return 0;
}
int Puff_Huffmen_Tree_growLeaf(struct Puff_Huffmen_Tree *self, uint16_t parentIndex, _Bool lr, uint16_t value) {

	if (value >> self->childBitLength) {
		return Puff_Huffmen_Tree_BADVALUE;
	}

	int e = 0;
	uint32_t parent32 = 0;
	e = mike_decompress_huffmen_tree_getNode32(self, parentIndex, &parent32);
	if (e) return e;

	uint16_t child = mike_decompress_huffmen_tree_Node32_shiftOut(parent32, lr, self->childBitLength);
	if (child) {
		return Puff_Huffmen_Tree_COLLISION;
	}

	mike_decompress_huffmen_tree_Node32_shiftIn(&parent32, lr, value, Puff_Huffmen_Tree_TYPE_LEAF, self->childBitLength);
	e = mike_decompress_huffmen_tree_setNode32(self, parentIndex, parent32);
	if (e) return e;

	return 0;
}

// lower
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

static inline int mike_decompress_huffmen_tree_getNode32(const struct Puff_Huffmen_Tree *self, uint16_t nodeIndex, uint32_t *destination) {
	if (nodeIndex >= self->nodeCount) {
		return Puff_Huffmen_Tree_OUTOFBOUNDS;
	}

	uint32_t n = 0;
	uint8_t *nodeData = self->data + (uint32_t)nodeIndex * self->nodeBytes;
	for (int i = 0; i < self->nodeBytes; ++i) {
		n |= (uint32_t)nodeData[i] << i * 8;
	}

	*destination = n;
	return 0;
}
static inline int mike_decompress_huffmen_tree_setNode32(struct Puff_Huffmen_Tree *self, uint16_t nodeIndex, uint32_t node32) {
	if (nodeIndex >= self->nodeCount) {
		return Puff_Huffmen_Tree_OUTOFBOUNDS;
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
