#ifndef PUFF_STATE_IMPL_H
#define PUFF_STATE_IMPL_H

#include "./iNostalgicWriter_impl.h"

#include "./huffmen/literalTree_impl.h"
#include "./huffmen/metaTree_impl.h"
#include "./huffmen/distanceTree_impl.h"

#include <stdint.h>

#define Puff_State_COLLECTFOR_LENGTH 0x80
#define Puff_State_COLLECTFOR_DISTANCE 0x81
#define Puff_State_COLLECTFOR_FIXEDDISTANCE 0x00

struct Puff_State {
	uint8_t id : 4;
	uint8_t isLastBlock : 1;
	uint8_t fixedTreeInitiated : 1; //TODO fixed tree should really be like a singleton type thing right?
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
		/*
			stores node index for any of the four trees below, depending on the rest of the state:
		*/

		struct Puff_LiteralTree fixed;
		struct Puff_LiteralTree dynamic;
		struct Puff_MetaTree meta;
		struct Puff_DistanceTree distance;
	} trees;

	struct Puff_State_BitCollector {
		uint16_t bits;
		/*
			stores the actual bits being collected
		*/

		uint8_t collected;
		/*
			number of bits that have been collected so far
		*/
		uint8_t max;
		/*
			number of bits to collect
		*/

		uint8_t collectFor;
		/*
			this & 0x80 == isLSBitOrder
		*/
	} collector;

	struct {
		uint16_t length;
		uint16_t distance;
	} lizard; // stores length/distance data as extra bits are counted up before being able to nostalgize


};

#endif
