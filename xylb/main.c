#include "./main.h"

#include "./error.h"
#include "./state_impl.h"

#include "./adleringWriter.h"

#include "puff/step.h"
#include "puff/error.h"
#include "puff/iNostalgicWriter.h"
#include "puff/iNostalgicWriter_impl.h"

#define STATE_HEADERING 0
#define STATE_DEFLATING 1
#define STATE_ADLERING 2
#define STATE_END 3

static inline int Xylb_decompress_doHeadering(struct Xylb_State *state, uint8_t byte);
static inline int Xylb_decompress_doDeflating(struct Xylb_State *state, uint8_t byte);
static inline int Xylb_decompress_doAdlering(struct Xylb_State *state, uint8_t byte);

int Xylb_decompress(struct Xylb_State *state, uint8_t byte) {
	switch (state->id) {
		case STATE_HEADERING:
			return Xylb_decompress_doHeadering(state, byte);
		case STATE_DEFLATING:
			return Xylb_decompress_doDeflating(state, byte);
		case STATE_ADLERING:
			return Xylb_decompress_doAdlering(state, byte);
		case STATE_END:
			return Xylb_decompress_END;
		default:
			return Xylb_decompress_IMPOSSIBLE;
	}
}

static inline int Xylb_decompress_doHeadering(struct Xylb_State *state, uint8_t byte) {

	uint16_t shirt = 0; //I dont remember why I called it shirt before but now I'm committed

	switch (state->bytesRead) {
		case 0:
			state->cminfo = byte;
			if ( (byte & 0x0f) != 8) {
				return Xylb_decompress_ERROR_COMPRESSIONMETHOD;
			}
			if ( ((byte & 0xf0) >> 4) > 7) {
				return Xylb_decompress_ERROR_WINDOWSIZE;
			}
			
			state->bytesRead++;
			break;
		case 1:
			shirt = state->cminfo;
			shirt <<= 8;
			shirt |= byte;

			if (shirt % 31) {
				return Xylb_decompress_ERROR_SHIRT;
			}
			if (byte & 0x20) {
				return Xylb_decompress_ERROR_HASDICT;
			}

			state->id = STATE_DEFLATING;
			state->bytesRead = 0;
			break;
		default:
			return Xylb_decompress_ERROR_HEADERING_BYTESREAD;
	}

	return 0;
}
static inline int Xylb_decompress_doAdlering(struct Xylb_State *state, uint8_t byte) {
	if (state->bytesRead > 3) {
		return Xylb_decompress_ERROR_ADLERING_BYTESREAD;
	}

	state->target <<= 8;
	state->target |= byte;

	if (state->bytesRead != 3) {
		state->bytesRead++;
		return 0;
	}

	uint32_t a = Xylb_AdleringWriter_getAdler(&state->adleringWriter);
	if (state->target != a) {
		return Xylb_decompress_ERROR_ADLER32;
	}

	state->id = STATE_END;
	return Xylb_decompress_END;
}

static inline int Xylb_decompress_doDeflating(struct Xylb_State *state, uint8_t byte) {

	int e = Puff_step(&state->puffState, byte);
	switch (e) {
		case 0:
			break;
		case Puff_step_END:
			state->id = STATE_ADLERING;
			break;
		default:
			return e + 100; //TODO TEMP
	}

	return 0;
}
