#include <stdio.h>

#include "./deflate.h"
#include "./deflate_impl.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define ZLIBHEADER 0
#define READINGADLER 7

#define BLOCKHEADER 1

#define UNCOMPRESSED 8

#define DECODINGSTATIC 2

#define BUILDINGMETATREE 3
#define BUILDINGMAINTREE 4
#define BUILDINGDISTANCETREE 5
#define DECODINGDYNAMIC 6

#define END 101


#define DIRECTIVE_FINISH_BYTE 1


static inline void mike_Deflate_clearData(Mike_Deflate_State *state);

int mike_Deflate_doZlibHeader(Mike_Deflate_State *state, uint8_t byte);
int mike_Deflate_doBlockHeader(Mike_Deflate_State *state, bool bit);
int mike_Deflate_doUncompressed(Mike_Deflate_State *state, uint8_t byte);


int Mike_Deflate_step(Mike_Deflate_State *state, uint8_t byte) { //do I need to return a bits-of-byte-read for the final thing?
	switch (state->id) {
		case END:
			return MIKE_DEFLATE_END;

		case ZLIBHEADER:
			return mike_Deflate_doZlibHeader(state, byte);
		case UNCOMPRESSED:
			return mike_Deflate_doUncompressed(state, byte);

		default: break;
	}

	int e = 0;
	for (int i = 1; i != 0; i = i << 1) {

		switch (state->id) { //These probably need to be bit-looped for all except ZLIBHEADER & ADLER32
			case END:
				return MIKE_DEFLATE_END;

			case BLOCKHEADER:
				e = mike_Deflate_doBlockHeader(state, byte & i);
				break;

			default: return MIKE_DEFLATE_ERROR_STATE_UNKNOWN;
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

int mike_Deflate_doZlibHeader(Mike_Deflate_State *state, uint8_t byte) {
	uint16_t shirt = 0;
	switch (state->data.zlibHeader.bytesRead) {
		case 0:
			state->data.zlibHeader.cminfo = byte;

			if ((byte & 0x0f) != 8) {
				return MIKE_DEFLATE_ERROR_ZLIBHEADER_COMPRESSIONMETHOD;
			}

			if (((byte & 0xf0) >> 4) > 7) {
				return MIKE_DEFLATE_ERROR_ZLIBHEADER_WINDOWSIZE;
			}
			state->windowSize = byte & 0x0f;


			state->data.zlibHeader.bytesRead++;
			break;
		case 1:
			shirt = state->data.zlibHeader.cminfo;
			shirt = shirt << 8;
			shirt = shirt | byte;

			if (shirt % 31) {
				return MIKE_DEFLATE_ERROR_ZLIBHEADER_VALIDATION;
			}
			if (byte & 0x20) {
				return MIKE_DEFLATE_ERROR_ZLIBHEADER_HASDICT;
			}


			state->id = BLOCKHEADER;
			mike_Deflate_clearData(state);
			break;
		default:
			return MIKE_DEFLATE_ERROR_ZLIBHEADER_OVERREAD;
	}
	return 0;
}

int mike_Deflate_doBlockHeader(Mike_Deflate_State *state, bool bit) {
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
					mike_Deflate_clearData(state);
					state->data.uncompressed.isLastBlock = bit;
					return DIRECTIVE_FINISH_BYTE;
				case 1: //fixed
					/*
					state->id = DECODINGSTATIC;
					bit = state->data.blockHeader.isLastBlock;
					mike_Deflate_clearData(state);
					state->data.decodingStatic.isLastBlock = bit;
					break;
					*/
				case 2: //dynamic
					/*
					state->id = BUILDINGMETATREE;
					bit = state->data.blockHeader.isLastBlock;
					mike_Deflate_clearData(state);
					state->data.buildingMetatree.isLastBlock = bit;
					break;
					*/
				default:
					return MIKE_DEFLATE_ERROR_BLOCKHEADER_COMPRESSIONTYPE;
			}

		default:
			return MIKE_DEFLATE_ERROR_BLOCKHEADER_OVERREAD;
	}
	return 0;
}

int mike_Deflate_doUncompressed(Mike_Deflate_State *state, uint8_t byte) {

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
					return MIKE_DEFLATE_ERROR_UNCOMPRESSED_NLEN;
				}

				state->data.uncompressed.bytesRead = 0;
				state->data.uncompressed.lengthObtained = true;
				break;

			default:
				return MIKE_DEFLATE_ERROR_UNCOMPRESSED_LENGTH_OVERREAD;
		}
		return 0;
	}

	if (state->data.uncompressed.bytesRead >= state->data.uncompressed.length) {
		if (state->data.uncompressed.isLastBlock) {
			state->id = END;
			return 0;
		}

		state->id = BLOCKHEADER;
		mike_Deflate_clearData(state);
		return 0;
	}

	//TODO actually send bytes somewhere

	state->data.uncompressed.bytesRead++;
	return 0;
}

static inline void mike_Deflate_clearData(Mike_Deflate_State *state) {
	memset(&state->data, 0, sizeof(state->data));
}
