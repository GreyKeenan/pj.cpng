#ifndef PUFF_STATE_IMPL_H
#define PUFF_STATE_IMPL_H

#include "./iNostalgicWriter_impl.h"

#include "./huffmen/literalTree_impl.h"
#include "./huffmen/metaTree_impl.h"
#include "./huffmen/distanceTree_impl.h"

#include <stdint.h>

struct Puff_State {
	uint8_t id : 4;
	uint8_t isLastBlock : 1;

	uint8_t compressionTypeBit0 : 1;
		//used by blockHeader
	uint8_t lengthObtained : 1;
		// used by UNCOMPRESSED state

	uint8_t fixedTreeInitiated : 1;

	uint8_t bitsRead;
	uint16_t bytesRead;
		// used by UNCOMPRESSED state

	uint16_t uncompressed_length;
	uint8_t uncompressed_invertedLengthByte0;

	struct Puff_iNostalgicWriter nostalgicWriter;


	struct Puff_LiteralTree fixedTree;
	/*
	struct Puff_LiteralTree dynamicTree;
	struct Puff_MetaTree metaTree;
	struct Puff_DistanceTree distanceTree;
	*/

	uint16_t currentNodeIndex;
};

#endif
