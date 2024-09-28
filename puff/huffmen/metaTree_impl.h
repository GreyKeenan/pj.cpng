#ifndef PUFF_METATREE_IMPL_H
#define PUFF_METATREE_IMPL_H

#include "./tree_impl.h"
#include <stdint.h>

#define Puff_MetaTree_MAXLEAVES 19
#define Puff_MetaTree_NODECOUNT (19 - 1)
#define Puff_MetaTree_NODEBYTES 2
#define Puff_MetaTree_LENGTH (18 * 2)

struct Puff_MetaTree {
	uint8_t data[Puff_MetaTree_LENGTH];
	struct Puff_Tree tree;
};

#endif
