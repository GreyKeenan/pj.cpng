#ifndef SHRUB_MetaTree_H
#define SHRUB_MetaTree_H

#include "./tree.h"

#define Shrub_MetaTree_MAXLEAVES 19
#define Shrub_MetaTree_BITSPERLEAF 5 //ceil(log2(MAXLEAVES))
#define Shrub_MetaTree_DATALENGTH Shrub_Tree_MEASURE(Shrub_MetaTree_MAXLEAVES, Shrub_MetaTree_BITSPERLEAF)

struct Gunc_BitStream;

struct Shrub_MetaTree {
	struct Shrub_Tree tree;
	uint8_t data[Shrub_MetaTree_DATALENGTH];
};

/*
static inline int Shrub_MetaTree_initTree(struct Shrub_MetaTree *self) {
	return Shrub_Tree_init(self->tree, self->data, Shrub_MetaTree_DATALENGTH, Shrub_MetaTree_MAXLEAVES, Shrub_MetaTree_BITSPERLEAF);
}
*/

int Shrub_MetaTree_init(struct Shrub_MetaTree *self, struct Gunc_BitStream *bis, uint8_t countGiven);



#endif
