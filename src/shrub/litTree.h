#ifndef SHRUB_LitTree_H
#define SHRUB_LitTree_H

#include "./tree.h"

#define Shrub_LitTree_MAXLEAVES 288
#define Shrub_LitTree_BITSPERLEAF 9 //ceil(log2(MAXLEAVES))
#define Shrub_LitTree_DATALENGTH Shrub_Tree_MEASURE(Shrub_LitTree_MAXLEAVES, Shrub_LitTree_BITSPERLEAF)

struct Shrub_LitTree {
	struct Shrub_Tree tree;
	uint8_t data[Shrub_LitTree_DATALENGTH];
};

static inline int Shrub_LitTree_init(struct Shrub_LitTree *self) {
	return Shrub_Tree_init(&self->tree, self->data, Shrub_LitTree_DATALENGTH, Shrub_LitTree_MAXLEAVES, Shrub_LitTree_BITSPERLEAF);
}

#endif
