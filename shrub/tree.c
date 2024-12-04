#include "./tree.h"

#include "./tree_core.h"

#include "gunc/log.h"

#include <stddef.h>
#include <math.h>

int Shrub_Tree_init(struct Shrub_Tree *self, uint8_t *data, uint16_t length, uint16_t maxLeaves) {
	if (data == NULL || self == NULL) {
		Gunc_err("null arg");
		return __LINE__;
	}

	if (maxLeaves < 2) {
		Gunc_err("too few leaves");
		return __LINE__;
	}
	uint16_t maxNodes = maxLeaves - 1;

	uint8_t bitsPerChild = ceil(log2(maxLeaves)) + 1;
	if (bitsPerChild > 16) {
		Gunc_err("children too large (%d bits)", bitsPerChild);
		return __LINE__;
	}

	uint8_t bytesPerNode = (bitsPerChild * 2) / 8 + (bool)((bitsPerChild * 2) % 8);
	if ((uint32_t)maxNodes * bytesPerNode > length) {
		Gunc_err("length (%d) cannot fit %d nodes of (%d) bytes each", length, maxNodes, bytesPerNode);
		return __LINE__;
	}

	*self = (struct Shrub_Tree) {
		.data = data,
		.maxNodes = maxNodes,
		.currentNodes = 1,
		.bitsPerChild = bitsPerChild,
		.bytesPerNode = bytesPerNode
	};

	Shrub_Tree_set(self, Shrub_Tree_ROOT, 0);
	return 0;
}

int Shrub_Tree_walk(const struct Shrub_Tree *self, bool isRight, uint16_t fromIndex, uint16_t *destination) {

	int e = 0;
	uint32_t from32 = 0;

	e = Shrub_Tree_get(self, fromIndex, &from32);
	if (e) {
		Gunc_nerr(e, "out of bounds");
		return Shrub_Tree_OUTOFBOUNDS;
	}

	uint16_t child = Shrub_Tree_shiftOut(from32, self->bitsPerChild, isRight);

	if (!child) {
		Gunc_err("child (%d) of (0x%x, index %d) is null", isRight, from32, fromIndex);
		return Shrub_Tree_HALT;
	}

	*destination = child;
	return 0;
}
int Shrub_Tree_birthNode(struct Shrub_Tree *self, bool isRight, uint16_t parent, uint16_t *nNewborn) {

	if (self->currentNodes >= self->maxNodes) {
		Gunc_err("too many nodes: %d / %d", self->currentNodes, self->maxNodes);
		return Shrub_Tree_TOOMUCH;
	}

	//get parent32
	//confirm null
	//increment currentNodes
	//set highest index as null
	//set parent32 value as newborn index
	//update parentIndex
	//update nNewborn

	Gunc_TODO("this");
	return 1;
}
int Shrub_Tree_growLeaf(struct Shrub_Tree *self, bool isRight, uint16_t parent, uint16_t value) {

	//validate value

	//get parent32
	//confirm null
	//set parent32 value
	//update parentIndex

	Gunc_TODO("this");
	return 1;
}
