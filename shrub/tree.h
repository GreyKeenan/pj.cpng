#ifndef SRHUB_Tree_H
#define SRHUB_Tree_H

#include <stdint.h>

#define Shrub_Tree_ROOT 0
	//ROOT doubles as NULL, when child is type NODE, since nothing points back to the root

#define Shrub_Tree_NODE 0
#define Shrub_Tree_LEAF 1

struct Shrub_Tree {
	uint8_t *data;
	uint16_t maxNodes;
	uint16_t currentNodes;
	uint8_t bitsPerChild; // including 1b type
	uint8_t bytesPerNode;
};


int Shrub_Tree_init(struct Shrub_Tree *tree, uint8_t *data, uint16_t length, uint16_t maxLeaves);

#endif
