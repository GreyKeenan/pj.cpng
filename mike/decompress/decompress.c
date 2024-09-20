#include <stdio.h>

#include "./decompress.h"

#include "./state_impl.h"

#include "./iNostalgicWriter.h"
#include "./iNostalgicWriter_forw.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

enum mike_decompress_State_Ids {
	ZLIBHEADER = 0
	, ADLER32

	, BLOCKHEADER
	
	, UNCOMPRESSED

	, DECODINGSTATIC

	, BUILDINGMETATREE
	, BUILDINGMAINTREE
	, BUILDINGDISTANCETREE
	, DECODINGDYNAMIC

	, END
};
#define DIRECTIVE_FINISH_BYTE 1

static inline void mike_decompress_clearData(mike_Decompress_State *state);

static inline int mike_decompress_write(mike_Decompress_State *state, uint8_t byte);
static inline int mike_decompress_nostalgize(mike_Decompress_State *state, uint8_t *destination, uint16_t distanceBack);

int mike_decompress_doZlibHeader(mike_Decompress_State *state, uint8_t byte);
int mike_decompress_doUncompressed(mike_Decompress_State *state, uint8_t byte);
int mike_decompress_doAdler32(mike_Decompress_State *state, uint8_t byte);

int mike_decompress_doBlockHeader(mike_Decompress_State *state, bool bit);


int mike_Decompress_step(mike_Decompress_State *state, uint8_t byte) { //do I need to return a bits-of-byte-read for the final thing?
	switch (state->id) {
		case END:
			return mike_Decompress_END;

		case ZLIBHEADER:
			return mike_decompress_doZlibHeader(state, byte);
		case UNCOMPRESSED:
			return mike_decompress_doUncompressed(state, byte);
		case ADLER32:
			return mike_decompress_doAdler32(state, byte);

		default: break;
	}

	int e = 0;
	for (int i = 1; i != 0; i = i << 1) {

		switch (state->id) { //These probably need to be bit-looped for all except ZLIBHEADER & ADLER32
			case END:
				return mike_Decompress_END;

			case BLOCKHEADER:
				e = mike_decompress_doBlockHeader(state, byte & i);
				break;

			default: return mike_Decompress_ERROR_STATE_UNKNOWN;
		}

		switch (e) {
			case 0: break;
			case DIRECTIVE_FINISH_BYTE:
				return 0;
			default: return e;
		}
	}

	return 0;
}

int mike_decompress_doZlibHeader(mike_Decompress_State *state, uint8_t byte) {
	uint16_t shirt = 0;
	switch (state->data.zlibHeader.bytesRead) {
		case 0:
			// init ADLER calcs
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			state->s1 = 1;
			state->s2 = 0;
			// do zlib header stuff
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			state->data.zlibHeader.cminfo = byte;

			if ((byte & 0x0f) != 8) {
				return mike_Decompress_ERROR_ZLIBHEADER_COMPRESSIONMETHOD;
			}

			if (((byte & 0xf0) >> 4) > 7) {
				return mike_Decompress_ERROR_ZLIBHEADER_WINDOWSIZE;
			}
			state->windowSize = byte & 0x0f;


			state->data.zlibHeader.bytesRead++;
			break;
		case 1:
			shirt = state->data.zlibHeader.cminfo;
			shirt = shirt << 8;
			shirt = shirt | byte;

			if (shirt % 31) {
				return mike_Decompress_ERROR_ZLIBHEADER_VALIDATION;
			}
			if (byte & 0x20) {
				return mike_Decompress_ERROR_ZLIBHEADER_HASDICT;
			}


			state->id = BLOCKHEADER;
			mike_decompress_clearData(state);
			break;
		default:
			return mike_Decompress_ERROR_ZLIBHEADER_OVERREAD;
	}
	return 0;
}

int mike_decompress_doBlockHeader(mike_Decompress_State *state, bool bit) {
	switch (state->data.blockHeader.bitsRead) {
		case 0:
			state->data.blockHeader.isLastBlock = bit;

			state->data.blockHeader.bitsRead++;
			break;
		case 1:
			state->data.blockHeader.compressionType = bit;

			state->data.blockHeader.bitsRead++;
			break;
		case 2:
			switch (state->data.blockHeader.compressionType | (bit << 1)) {
				case 0: //none
					state->id = UNCOMPRESSED;
					bit = state->data.blockHeader.isLastBlock;
					mike_decompress_clearData(state);
					state->data.uncompressed.isLastBlock = bit;
					return DIRECTIVE_FINISH_BYTE;
				case 1: //fixed
					/*
					state->id = DECODINGSTATIC;
					bit = state->data.blockHeader.isLastBlock;
					mike_decompress_clearData(state);
					state->data.decodingStatic.isLastBlock = bit;
					break;
					*/
				case 2: //dynamic
					/*
					state->id = BUILDINGMETATREE;
					bit = state->data.blockHeader.isLastBlock;
					mike_decompress_clearData(state);
					state->data.buildingMetatree.isLastBlock = bit;
					break;
					*/
				default:
					return mike_Decompress_ERROR_BLOCKHEADER_COMPRESSIONTYPE;
			}

		default:
			return mike_Decompress_ERROR_BLOCKHEADER_OVERREAD;
	}
	return 0;
}

int mike_decompress_doUncompressed(mike_Decompress_State *state, uint8_t byte) {

	if (!state->data.uncompressed.lengthObtained) {
		switch (state->data.uncompressed.bytesRead) {
			case 0:
				state->data.uncompressed.length = byte;

				state->data.uncompressed.bytesRead++;
				break;
			case 1:
				state->data.uncompressed.length = state->data.uncompressed.length | (uint16_t)byte << 8;

				state->data.uncompressed.bytesRead++;
				break;
			case 2:
				state->data.uncompressed.invertedLength = byte;

				state->data.uncompressed.bytesRead++;
				break;
			case 3:

				if (state->data.uncompressed.length != (uint16_t)~(state->data.uncompressed.invertedLength | (uint16_t)byte << 8)) {
					return mike_Decompress_ERROR_UNCOMPRESSED_NLEN;
				}

				state->data.uncompressed.bytesRead = 0;
				state->data.uncompressed.lengthObtained = true;
				break;

			default:
				return mike_Decompress_ERROR_UNCOMPRESSED_LENGTH_OVERREAD;
		}
		return 0;
	}


	mike_decompress_write(state, byte);

	state->data.uncompressed.bytesRead++;
	if (state->data.uncompressed.bytesRead >= state->data.uncompressed.length) {
		if (state->data.uncompressed.isLastBlock) {

			mike_decompress_clearData(state);
			state->id = ADLER32;
			return 0;
		}

		state->id = BLOCKHEADER;
		mike_decompress_clearData(state);
		return 0;
	}
	return 0;
}

int mike_decompress_doAdler32(mike_Decompress_State *state, uint8_t byte) {
	if (state->data.adler32.bytesRead > 3) {
		return mike_Decompress_ERROR_ADLER32_OVERREAD;
	}

	state->data.adler32.target += byte << (8 * (3 - state->data.adler32.bytesRead));

	if (state->data.adler32.bytesRead != 3) {
		state->data.adler32.bytesRead++;
		return 0;
	}

	uint32_t a = (state->s2 << 16) + state->s1;
	//printf("target: %x / have: %x\n", state->data.adler32.target, a);
	if (state->data.adler32.target != a) {
		return mike_Decompress_ERROR_ADLER32_FAILED;
	}

	state->id = END;
	return mike_Decompress_END; //returns END on completion, rather than only returning when calling past completion
}


// utilities
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

static inline void mike_decompress_clearData(mike_Decompress_State *state) {
	memset(&state->data, 0, sizeof(state->data));
}


#define BASE 65521
static inline void mike_decompress_adler32(mike_Decompress_State *state, uint8_t byte) {
	state->s1 = (state->s1 + byte) % BASE;
	state->s2 = (state->s1 + state->s2) % BASE;

}
static inline int mike_decompress_write(mike_Decompress_State *state, uint8_t byte) {

	if (Mike_Decompress_iNostalgicWriter_write(state->nw, byte)) {
		return mike_Decompress_ERROR_WRITER_WRITE;
	}
	mike_decompress_adler32(state, byte);

	return 0;
}
static inline int mike_decompress_nostalgize(mike_Decompress_State *state, uint8_t *destination, uint16_t distanceBack) {

	if (Mike_Decompress_iNostalgicWriter_nostalgize(state->nw, destination, distanceBack)) {
		return mike_Decompress_ERROR_WRITER_NOSTALGIZE;
	}

	return 0;
}

