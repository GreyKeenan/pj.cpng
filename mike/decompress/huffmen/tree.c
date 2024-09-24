
#include "./tree.h"
#include "./tree_impl.h"

#include <stddef.h>
#include <math.h>


static inline int mike_decompress_huffmen_tree_readEntireNode(struct Mike_Decompress_Huffmen_Tree *self, uint16_t node, uint32_t *destination);
static inline int mike_decompress_huffmen_tree_setChildValue(struct Mike_Decompress_Huffmen_Tree * self, uint16_t node, _Bool side, _Bool type, uint16_t value);


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
	if (!cap || !maxNodeCount || !nodeBytes || !childBitLength) {
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
	return -10;
}
int Mike_Decompress_Huffmen_Tree_birthChild(struct Mike_Decompress_Huffmen_Tree *self, uint16_t *fromNode, _Bool handedness) {
	return -10;
}
int Mike_Decompress_Huffmen_Tree_setLeafChild(struct Mike_Decompress_Huffmen_Tree *self, uint16_t fromNode, _Bool handedness, uint16_t value) {
	return -10;
}


static inline int mike_decompress_huffmen_tree_readEntireNode(struct Mike_Decompress_Huffmen_Tree *self, uint16_t node, uint32_t *destination) {
	return -11;
}
static inline int mike_decompress_huffmen_tree_setChildValue(struct Mike_Decompress_Huffmen_Tree * self, uint16_t node, _Bool side, _Bool type, uint16_t value) {
	return -11;
}
