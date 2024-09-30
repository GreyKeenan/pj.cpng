#ifndef PUFF_STATE_IMPL_H
#define PUFF_STATE_IMPL_H

#include "./iNostalgicWriter_impl.h"

#include "./huffmen/literalTree_impl.h"
#include "./huffmen/metaTree_impl.h"
#include "./huffmen/distanceTree_impl.h"

#include <stdint.h>

#define Puff_State_COLLECTFOR_LENGTH 0
#define Puff_State_COLLECTFOR_DISTANCE 1
#define Puff_State_COLLECTFOR_FIXEDDISTANCE 2

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
			used to store node index for any of the four trees below, depending on the rest of the state:
		*/

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
			1+ indicates to collect bits, then decrease by 1
				length extra bits: 1-5
				fixed-ct distance code bits: 1-5
				distance extra bits: 1-13
		*/
		uint8_t maxCollect;
		/*
			a count of the num of bits that will be collected
			(when you set collect, also set totalCollected to the same value)
			maybe a bit messy but am using it for reversing the bit order
		*/
		uint8_t collectFor;
		/*
			0 = length code extra bits
			1 = distance code extra bits
			2 = fixed-ct distance code bits (its a 5-bit ~~uint~~ CODE NOT INT)
		*/
	} extraBits; // vars for reading extra bits after length/distance codes & for fixed tree distance codes
	struct {
		uint16_t length;
		uint16_t distance;
	} lizard; // stores length/distance data as extra bits are counted up before being able to nostalgize


};

#endif
