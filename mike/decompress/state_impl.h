#ifndef MIKE_DECOMPRESS_IMPL_H
#define MIKE_DECOMPRESS_IMPL_H

#include "./iNostalgicWriter_forw.h"

#include <stdint.h>

struct Mike_Decompress_State {

	union {
		struct { uint8_t bytesRead; uint8_t cminfo; } zlibHeader; //first for 0 init
		struct {
			uint8_t bitsRead;
			uint8_t compressionType;
			_Bool isLastBlock;
		} blockHeader;

		struct {
			_Bool isLastBlock;
			_Bool lengthObtained;
			uint16_t bytesRead;
			uint16_t length;
			uint16_t invertedLength;
		} uncompressed;

		uint16_t currentCode;
		struct { uint32_t target; uint8_t bytesRead; } adler32;
	} data;
	uint8_t id;
	/*
		zlibHeader

		deflateBlockHeader

		decodingStatic

		buildingMetaTree
		buildingMainTree
		buildingDistanceTree
		decodingDynamic

		readingAdler32
	*/
	uint16_t windowSize;
	uint32_t s1;
	uint32_t s2;

	//staticTree //may need to be ptrs for padding, but nodes will probably be smaller than the union anyawys
	//dynamicTree
	//metaTree
	//distanceTree
	
	struct Mike_Decompress_iNostalgicWriter *nw;

};

#endif
