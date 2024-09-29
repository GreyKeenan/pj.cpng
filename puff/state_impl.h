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
	uint8_t fixedTreeInitiated : 1; //TODO just initiate it when the State is created
	struct Puff_iNostalgicWriter nostalgicWriter;
	
	struct {
		uint8_t bitsRead;
		_Bool compressionTypeBit0;
	} blockHeader;

	struct {
		_Bool lengthObtained;
		uint8_t invertedLengthLSB;
		uint16_t length;
		uint16_t bytesRead;
	} uncompressed;

	struct {
		uint16_t nodeIndex;

		struct Puff_LiteralTree fixed;
		struct Puff_LiteralTree dynamic;
		struct Puff_MetaTree meta;
		struct Puff_DistanceTree distance;
	} trees;
	struct {
		uint16_t bits;
		uint8_t collect;
		/*
			0 = not currently collecting bits
			1 + indicates to collect bits, then decrease by 1
				length extra bits: 1-5
				fixed-ct distance code bits: 1-5
				distance extra bits: 1-13
		*/
		uint8_t collectFor;
		/*
			0 = length code extra bits
			1 = distance code extra bits
			2 = fixed-ct distance code bits (its a 5-bit uint)
		*/
	} extraBits;


};

#endif
