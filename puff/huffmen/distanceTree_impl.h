#ifndef PUFF_DISTANCETREE_IMPL_H
#define PUFF_DISTANCETREE_IMPL_H

#include "./tree_impl.h"
#include <stdint.h>

#define Puff_DistanceTree_MAXLEAVES 32
#define Puff_DistanceTree_NODECOUNT (32 - 1)
#define Puff_DistanceTree_NODEBYTES 2
#define Puff_DistanceTree_LENGTH (31 * 2)

struct Puff_DistanceTree {
	uint8_t data[Puff_DistanceTree_LENGTH];
	struft Puff_Tree tree;
};

#endif
