#ifndef PUFF_STATE_IMPL_H
#define PUFF_STATE_IMPL_H

#include "./iNostalgicWriter_impl.h"

#include "./huffmen/literalTree_impl.h"
#include "./huffmen/metaTree_impl.h"
#include "./huffmen/distanceTree_impl.h"

#include <stdint.h>

enum {
	Puff_State_ID_BLOCKHEADER = 0
	, Puff_State_ID_UNCOMPRESSED = 1
	, Puff_State_ID_FIXED = 2
	, Puff_State_ID_DYNAMIC = 3
	, Puff_State_ID_END = 4
};

enum {

	Puff_State_COLLECTFOR_MSBIT = 0x00

	, Puff_State_COLLECTFOR_FIXEDDISTANCE


	, Puff_State_COLLECTFOR_LSBIT = 0x80

	, Puff_State_COLLECTFOR_LENGTH
	, Puff_State_COLLECTFOR_DISTANCE
	, Puff_State_COLLECTFOR_DYNAMIC_MEASURING
	, Puff_State_COLLECTFOR_DYNAMIC_META
};

enum Puff_State_Dynamic_focus {
	Puff_State_DYNAMIC_UNINITIALIZED = 0
	, Puff_State_DYNAMIC_MAIN
	, Puff_State_DYNAMIC_DIST
	, Puff_State_DYNAMIC_META
};

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

	struct Puff_State_Dynamic {
		uint8_t focus : 2;
		/*
			0 = first call, need to initialize
			1 = mainTree
			2 = distanceTree
			3 = metaTree
		*/

		uint16_t unitsRead;

		uint8_t repeat;
		/*
			for meta tree repeat codes
			stores the codeLength value to be repeated once bits are collected
		*/

		uint16_t codeLengthCount_main; //can be uint8 & add 256 every time
		uint8_t codeLengthCount_meta;
		uint8_t codeLengthCount_dist;

		union {
			uint8_t meta[Puff_MetaTree_MAXLEAVES];
			uint8_t maindist[Puff_LiteralTree_MAXLEAVES + Puff_DistanceTree_MAXLEAVES];
			/*
			stores the codelengths for the trees
			*/
		} lengths;

	} dynamic;

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
