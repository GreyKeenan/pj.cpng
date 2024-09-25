
#include "./tree.h"
#include "./tree_impl.h"

#include <stddef.h>
#include <math.h>

#define IMPOSSIBLE -255


static inline int mike_decompress_huffmen_tree_readEntireNode(const struct Mike_Decompress_Huffmen_Tree *self, uint16_t node, uint32_t *destination);
static inline int mike_decompress_huffmen_tree_setEntireNode(struct Mike_Decompress_Huffmen_Tree *self, uint16_t nodeIndex, uint32_t node);

static inline uint16_t mike_decompress_huffmen_tree_shiftOut(uint16_t node, _Bool handedness, uint8_t bitLength) {
	return (node >> ((bitLength + 1) * handedness)) & ((1 << bitLength + 1) - 1);
}
static inline uint32_t mike_decompress_huffmen_tree_setChildValue(uint32_t initialNode, _Bool handedness, uint16_t value, uint8_t bitLength) {
	bitLength++;
	initialNode &= ((1 << bitLength) - 1) << bitLength * !handedness;
	initialNode |= value << bitLength * handedness;
	return initialNode;
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

	*self = (struct Mike_Decompress_Huffmen_Tree) {
		.data = data,
		.maxNodeCount = maxNodeCount,
		.nodeCount = 1,
		.nodeBytes = nodeBytes,
		.childBitLength = childBitLength
	};
	return mike_decompress_huffmen_setEntireNode(self, Mike_Decompress_Huffmen_Tree_ROOT, 0);
}

int Mike_Decompress_Huffmen_Tree_walk(const struct Mike_Decompress_Huffmen_Tree *self, uint16_t fromNode, _Bool handedness, uint16_t *destination) {
	int e = 0;
	uint32_t entire_fromNode = 0;

	e = mike_decompress_huffmen_tree_readEntireNode(self, fromNode, &entire_fromNode);
	if (e) return e;

	uint16_t child = mike_decompress_huffmen_tree_shiftOut(entire_fromNode, handedness, self->childBitLength);

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

	//check for room
	if (self->nodeCount >= self->maxNodeCount) {
		return Mike_Decompress_Huffmen_Tree_TOOMANYKIDS;
	}

	//read fromNode
	int e = 0;
	uint32_t entire_fromNode = 0;
	e = mike_decompress_huffmen_tree_readEntireNode(self, *fromNode, &entire_fromNode);
	if (e) return e;

	//get relevant child of fromNode & check if null
	uint16_t child = mike_decompress_huffmen_tree_shiftOut(entire_fromNode, handednes, self->childBitLength);
	if (child) {
		return Mike_Decompress_Huffmen_Tree_COLLISION;
	}
	child >>= 1;

	//create a new node at the end of the array of data
	self->nodeCount++;
	e = huffmen_tree_setEntireNode(self, self->nodeCount - 1, 0);
	if (e) return IMPOSSIBLE;

	//set fromNode's child value to the index of the new node
	uint16_t value = self->nodeCount - 1 << 1;
	entire_fromNode = mike_decompress_huffmen_tree_setChildValue(entire_fromNode, handedness, value, self->childBitLength);
	e = mike_decompress_huffmen_tree_setEntireNode(self, *fromNode, entire_fromNode);
	if (e) return IMPOSSIBLE;

	//give the index of the new node back
	*fromNode = self->nodeCount - 1;
	return 0;
}
int Mike_Decompress_Huffmen_Tree_setLeafChild(struct Mike_Decompress_Huffmen_Tree *self, uint16_t fromNode, _Bool handedness, uint16_t value) {

	//read fromNode
	int e = 0;
	uint32_t entire_fromNode = 0;
	e = mike_decompress_huffmen_tree_readEntireNode(self, fromNode, &entire_fromNode);
	if (e) return e;

	//get relevant child of fromNode & check if null
	uint16_t child = mike_decompress_huffmen_tree_shiftOut(entire_fromNode, handednes, self->childBitLength);
	if (child) {
		return Mike_Decompress_Huffmen_Tree_COLLISION;
	}
	child >>= 1;

	//set fromNode's child value to the new value
	value = (value << 1) | 1;
	entire_fromNode = mike_decompress_huffmen_tree_setChildValue(entire_fromNode, handedness, value, self->childBitLength);
	e = mike_decompress_huffmen_tree_setEntireNode(self, fromNode, entire_fromNode);
	if (e) return IMPOSSIBLE;

	return 0;
}


static inline int mike_decompress_huffmen_tree_readEntireNode(const struct Mike_Decompress_Huffmen_Tree *self, uint16_t node, uint32_t *destination) {
	if (node >= self->nodeCount) {
		return Mike_Decompress_Huffmen_Tree_OUTOFBOUNDS;
	}

	uint32_t n = 0;
	uint8_t *nodeData = self->data + (uint32_t)node * self->nodeBytes;
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

	uint8_t *nodeData = self->data + (uint32_t)nodeIndex * self->nodeBytes;
	for (int i = 0; i < self->nodeBytes; ++i) {
		nodeData[i] = (node >> (i * 8)) & 0xff;
	}

	return 0;
}

