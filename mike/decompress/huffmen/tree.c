
#include "./tree.h"
#include "./tree_impl.h"

#include <stddef.h>
#include <math.h>


static inline int mike_decompress_huffmen_tree_readEntireNode(const struct Mike_Decompress_Huffmen_Tree *self, uint16_t node, uint32_t *destination);
static inline int mike_decompress_huffmen_tree_setEntireNode(struct Mike_Decompress_Huffmen_Tree *self, uint16_t nodeIndex, uint32_t node);

static inline uint16_t mike_decompress_huffmen_tree_shiftOutLeft(const struct Mike_Decompress_Huffmen_Tree *self, uint32_t entireNode) {
	return (entireNode >> (self->childBitLength + 1)) & ((1 >> (self->childBitLength + 1)) - 1);
}
static inline uint16_t mike_decompress_huffmen_tree_shiftOutRight(const struct Mike_Decompress_Huffmen_Tree *self, uint32_t entireNode) {
	return entireNode & ((1 >> (self->childBitLength + 1)) - 1);
}


int Mike_Decompress_Huffmen_Tree_init(struct Mike_Decompress_Huffmen_Tree *self, uint8_t *data, uint16_t cap, uint16_t uniqueValueCount, uint8_t nodeBytes, uint8_t childBitLength) {

	uint16_t maxNodeCount = uniqueValueCount - 1;
	if (!uniqueValueCount) {
		return 7;
	}
	if (uniqueValueCount >> childBitLength) {
		return 6;
	}

	if (data == NULL) {
		return 1;
	}
	if (!maxNodeCount || !nodeBytes || !childBitLength) {
		return 2;
	}
	if (nodeBytes > 4) {
		return 3;
	}
	if (ceil((childBitLength * 2 + 2) /(float) 8) > nodeBytes) {
		return 4;
	}
	if (cap < maxNodeCount * nodeBytes) {
		return 5;
	}

	//set both ROOT's children as null
	for (int i = 0; i < nodeBytes; ++i) {
		data[i] = 0;
	}

	*self = (struct Mike_Decompress_Huffmen_Tree) {
		.data = data,
		.maxNodeCount = maxNodeCount,
		.nodeCount = 1,
		.nodeBytes = nodeBytes,
		.childBitLength = childBitLength
	};
	return 0;
}

int Mike_Decompress_Huffmen_Tree_walk(const struct Mike_Decompress_Huffmen_Tree *self, uint16_t fromNode, _Bool handedness, uint16_t *destination) {
	int e = 0;
	uint32_t entire_fromNode = 0;

	e = mike_decompress_huffmen_tree_readEntireNode(self, fromNode, &entire_fromNode);
	if (e) return e;

	uint16_t child = 0;

	switch (handedness) {
		case 0:
			child = mike_decompress_huffmen_tree_shiftOutLeft(self, entire_fromNode);
			break;
		case 1:
			child = mike_decompress_huffmen_tree_shiftOutRight(self, entire_fromNode);
			break;
	}

	_Bool type_isValue = child & 1;
	child = child >> 1;

	if (type_isValue) {
		*destination = child;
		return Mike_Decompress_Huffmen_Tree_ISLEAF;
	}

	if (child == Mike_Decompress_Huffmen_Tree_ROOT) {
		return Mike_Decompress_Huffmen_Tree_HALT;
	}

	*destination = child;
	return Mike_Decompress_Huffmen_Tree_ISNODE;
}
int Mike_Decompress_Huffmen_Tree_birthChild(struct Mike_Decompress_Huffmen_Tree *self, uint16_t *fromNode, _Bool handedness) {
	return -10;
}
int Mike_Decompress_Huffmen_Tree_setLeafChild(struct Mike_Decompress_Huffmen_Tree *self, uint16_t fromNode, _Bool handedness, uint16_t value) {
	return -10;
}


static inline int mike_decompress_huffmen_tree_readEntireNode(const struct Mike_Decompress_Huffmen_Tree *self, uint16_t node, uint32_t *destination) {
	if (node >= self->nodeCount) {
		return Mike_Decompress_Huffmen_Tree_OUTOFBOUNDS;
	}

	uint32_t n = 0;
	uint8_t *nodeData = self->data + node * self->nodeBytes;

	for (int i = 0; i < self->nodeBytes; ++i) {
		n = (n << 8) | nodeData[i];
	}

	*destination = n;
	return 0;
}
static inline int mike_decompress_huffmen_tree_setEntireNode(struct Mike_Decompress_Huffmen_Tree *self, uint16_t nodeIndex, uint32_t node) {
/*
	doesnt validate uint32 node value. Just grabs the bits.
*/
	if (node >= self->nodeCount) {
		return Mike_Decompress_Huffmen_Tree_OUTOFBOUNDS;
	}

	uint8_t *nodeData = self->data + nodeIndex * self->nodeBytes;
	nodeData += self->nodeBytes - 1; //point at last byte aka LSB

	for (int i = 0; i < self->nodeBytes; ++i) {
		*nodeData = (node >> (i * 8)) & 0xff;
		nodeData--;
	}

	return 0;
}

