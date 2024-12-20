#ifndef SHRUB_DistTree_H
#define SHRUB_DistTree_H

#include "./tree.h"

#define Shrub_DistTree_MAXLEAVES 32
#define Shrub_DistTree_BITSPERLEAF 5 //ceil(log2(MAXLEAVES))
#define Shrub_DistTree_DATALENGTH Shrub_Tree_MEASURE(Shrub_DistTree_MAXLEAVES, Shrub_DistTree_BITSPERLEAF)

struct Shrub_DistTree {
	struct Shrub_Tree tree;
	uint8_t data[Shrub_DistTree_DATALENGTH];
};


/*
static inline int Shrub_DistTree_init(struct Shrub_DistTree *self) {
	return Shrub_Tree_init(&self->tree, self->data, Shrub_DistTree_DATALENGTH, Shrub_DistTree_MAXLEAVES, Shrub_DistTree_BITSPERLEAF);
}
*/

int Shrub_DistTree_init(struct Shrub_DistTree *self, uint8_t *sizes, uint8_t countGiven);

#endif
