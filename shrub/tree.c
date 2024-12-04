#include "./tree.h"

#include "./tree_core.h"

#include "gunc/log.h"

#include <stddef.h>
#include <math.h>


/*
uint16_t Shrub_Tree_measure(uint16_t maxLeaves, uint8_t bitsPerLeaf) {
	if (maxLeaves < 2) {
		Gunc_err("too few leaves");
		return __LINE__;
	}
	uint16_t maxNodes = maxLeaves - 1;

	if (!bitsPerLeaf) {
		bitsPerLeaf = ceil(log2(maxLeaves));
	}
	uint8_t bitsPerChild = bitsPerLeaf + 1;
	if (bitsPerChild > 16) {
		Gunc_err("children too large (%d bits)", bitsPerChild);
		return 0;
	}

	uint8_t bytesPerNode = (bitsPerChild * 2) / 8 + (bool)((bitsPerChild * 2) % 8);

	uint32_t length = (uint32_t)maxNodes * bytesPerNode;
	if (length > UINT16_MAX) {
		Gunc_err("length too large: %d", length);
		return 0;
	}

	return length;
}
*/

int Shrub_Tree_init(struct Shrub_Tree *self, uint8_t *data, uint16_t length, uint16_t maxLeaves, uint8_t bitsPerLeaf) {
	if (data == NULL || self == NULL) {
		Gunc_err("null arg");
		return __LINE__;
	}

	if (maxLeaves < 2) {
		Gunc_err("too few leaves");
		return __LINE__;
	}
	uint16_t maxNodes = maxLeaves - 1;

	if (!bitsPerLeaf) {
		bitsPerLeaf = ceil(log2(maxLeaves));
	}
	uint8_t bitsPerChild = bitsPerLeaf + 1;
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

	if (Shrub_Tree_set(self, Shrub_Tree_ROOT, 0)) {
		return __LINE__;
	}
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
		Gunc_warn("child (%d) of (0x%x, index %d) is null", isRight, from32, fromIndex);
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

	int e = 0;
	uint32_t parent32 = 0;

	e = Shrub_Tree_get(self, parent, &parent32);
	if (e) {
		Gunc_nerr(e, "out of bounds");
		return Shrub_Tree_OUTOFBOUNDS;
	}

	uint16_t child = Shrub_Tree_shiftOut(parent32, self->bitsPerChild, isRight);
	if (child) {
		Gunc_err("collision, non-null child(%d): 0x%x", isRight, parent32);
		return Shrub_Tree_HALT;
	}

	uint16_t newbornIndex = self->currentNodes;
	self->currentNodes++;
	if (Shrub_Tree_set(self, newbornIndex, 0)) {
		return Shrub_Tree_ERROR;
	}

	Shrub_Tree_shiftIn(&parent32, self->bitsPerChild, isRight, newbornIndex, Shrub_Tree_NODE);
	if (Shrub_Tree_set(self, parent, parent32)) {
		return Shrub_Tree_ERROR;
	}

	if (nNewborn != NULL) {
		*nNewborn = newbornIndex;
	}

	return 0;
}
int Shrub_Tree_growLeaf(const struct Shrub_Tree *self, bool isRight, uint16_t parent, uint16_t value) {

	if (value >> (self->bitsPerChild - 1)) {
		Gunc_err("bad value: %d", value);
		return Shrub_Tree_TOOMUCH;
	}

	int e = 0;
	uint32_t parent32 = 0;

	e = Shrub_Tree_get(self, parent, &parent32);
	if (e) {
		Gunc_nerr(e, "out of bounds");
		return Shrub_Tree_OUTOFBOUNDS;
	}

	uint16_t child = Shrub_Tree_shiftOut(parent32, self->bitsPerChild, isRight);
	if (child) {
		Gunc_err("collision, non-null child(%d): 0x%x", isRight, parent32);
		return Shrub_Tree_HALT;
	}

	Shrub_Tree_shiftIn(&parent32, self->bitsPerChild, isRight, value, Shrub_Tree_LEAF);
	if (Shrub_Tree_set(self, parent, parent32)) {
		return Shrub_Tree_ERROR;
	}

	return 0;
}


int Shrub_Tree_enterCode(struct Shrub_Tree *self, uint16_t code, uint16_t length, uint16_t leaf) {

	Gunc_say("entering leaf (%d) for code (0x%x) of length %d", leaf, code, length);

	if (length < 1 || 16 < length) { // fit within int16
		Gunc_err("invalid codelength: %d", length);
		return Shrub_Tree_BADCODE;
	}
	if (length > self->maxNodes) { // fit within tree.data
		Gunc_err("codelength (%d) > maxNodes (%d)", length, self->maxNodes);
		return Shrub_Tree_BADCODE;
	}
	// could validate leaf here too

	int e = 0;
	uint16_t currentIndex = Shrub_Tree_ROOT;
	uint16_t child = 0;

	for (
		uint16_t i = 1 << (length - 1);
		i != 0;
		i >>= 1
	) {
		if (i == 1) {
			e = Shrub_Tree_growLeaf(self, code & i, currentIndex, leaf);
			if (e) {
				Gunc_nerr(e, "failed grow");
				return e;
			}
			break;
		}

		e = Shrub_Tree_walk(self, code & i, currentIndex, &child);
		switch (e) {
			case 0: // non-null value
				if (child & Shrub_Tree_LEAF) {
					Gunc_err("collision. Leaf already exists.");
					return Shrub_Tree_HALT;
				}
				currentIndex = child >> 1;
				break;
			case Shrub_Tree_HALT: // null, so make new node
				e = Shrub_Tree_birthNode(self, code & i, currentIndex, &child);
				if (e) {
					Gunc_nerr(e, "failed birth");
					return e;
				}
				currentIndex = child;
				break;

			default: // other errors
				Gunc_nerr(e, "failed walk");
				return e;
		}
	}

	return 0;
}
