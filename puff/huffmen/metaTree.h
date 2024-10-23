#ifndef PUFF_METATREE_H
#define PUFF_METATREE_H

#include "./metaTree_impl.h"

extern const uint8_t Puff_MetaTree_LENGTHSORDER[Puff_MetaTree_MAXLEAVES];

int Puff_MetaTree_init(struct Puff_MetaTree *self, const uint8_t lengths[Puff_MetaTree_MAXLEAVES], uint8_t lengthsLength);
/*
	builds the metaTree based on the given list of codeLengthCounts

	asserts lengths[any] <= MAXLENGTH
	asserts lengthsLength > 0 && lengthsLength < Puff_MetaTree_MAXLEAVES

	returns 0 on success
*/

#endif
