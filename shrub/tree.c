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
