#ifndef PUFF_LITERALTREE_IMPL_H
#define PUFF_LITERALTREE_IMPL_H

#include "./tree_impl.h"
#include <stdint.h>

#define Puff_LiteralTree_MAXLEAVES 288
#define Puff_LiteralTree_NODECOUNT (288 - 1)
#define Puff_LiteralTree_NODEBYTES 3
#define Puff_LiteralTree_LENGTH (287 * 3)

struct Puff_LiteralTree {
	uint8_t data[Puff_LiteralTree_LENGTH];
	struct Puff_Tree tree;
};

#endif
