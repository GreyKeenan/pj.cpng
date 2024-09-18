#include <stdio.h>

#include "./deflate.h"
#include "./deflate_impl.h"

#include <stdint.h>
#include <string.h>

#define ZLIBHEADER 0
#define BLOCKHEADER 1
#define DECODINGSTATIC 2
#define BUILDINGMETATREE 3
#define BUILDINGMAINTREE 4
#define BUILDINGDISTANCETREE 5
#define DECODINGDYNAMIC 6
#define READINGADLER 7

#define END 101


static inline void mike_Deflate_clearData(Mike_Deflate_State *state);

int Mike_Deflate_doZlibHeader(Mike_Deflate_State *state, uint8_t byte);


int Mike_Deflate_step(Mike_Deflate_State *state, uint8_t byte) {
	switch (state->id) {
		case ZLIBHEADER:
			return Mike_Deflate_doZlibHeader(state, byte);
		case END:
			return MIKE_DEFLATE_END;
		default: return MIKE_DEFLATE_ERROR_STATE_UNKNOWN;
	}
	return 0;
}

int Mike_Deflate_doZlibHeader(Mike_Deflate_State *state, uint8_t byte) {
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

static inline void mike_Deflate_clearData(Mike_Deflate_State *state) {
	memset(&state->data, 0, sizeof(state->data));
}
