#ifndef MIKE_DEFLATE_IMPL_H
#define MIKE_DEFLATE_IMPL_H

#include <stdint.h>

struct Mike_Deflate_State {

	union {
		struct { uint8_t bytesRead; uint8_t cminfo; } zlibHeader;
			//if first^, avoids issues with 0 initialization before it is memset 0 later
		uint8_t bitsRead;
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
	uint32_t runningAdler;

	//staticTree //may need to be ptrs for padding, but nodes will probably be smaller than the union anyawys
	//dynamicTree
	//metaTree
	//distanceTree

};

#endif
