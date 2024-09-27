#include <stdio.h>

#include "./main.h"

#include "./error.h"
#include "./state_impl.h"

#include "./iNostalgicWriter.h"

#define STATE_BLOCKHEADER 0
#define STATE_UNCOMPRESSED 1
#define STATE_FIXED 2
#define STATE_DYNAMIC 3
#define STATE_END 4

static inline int Puff_step_doBlockHeader(struct Puff_State *state, _Bool bit);
static inline int Puff_step_doUncompressed(struct Puff_State *state, uint8_t byte);
static inline int Puff_step_doFixed(struct Puff_State *state, _Bool bit);
static inline int Puff_step_doDynamic(struct Puff_State *state, _Bool bit);

int Puff_step(struct Puff_State *state, uint8_t byte) {

	int e = 0;
	_Bool bit = 0;

	for (int i = 1; i != 0; i <<= 1) {

		bit = byte & i;

		switch (state->id) {
			case STATE_END:
				return Puff_step_END;
			case STATE_BLOCKHEADER:
				e = Puff_step_doBlockHeader(state, bit);
				break;
			case STATE_FIXED:
				e = Puff_step_doFixed(state, bit);
				break;
			case STATE_DYNAMIC:
				e = Puff_step_doDynamic(state, bit);
				break;
			case STATE_UNCOMPRESSED:
				if (i != 1) {
					return Puff_step_ERROR_UNALIGNEDUNCOMPRESSED;
				}
				return Puff_step_doUncompressed(state, byte);
			default:
				return Puff_step_ERROR_STATE;
		}

		switch (e) {
			case 0:
				break;
			case Puff_step_DIRECTIVE_FINISHBYTE:
				return 0;
			default:
				return e;
		}
	}

	return 0;
}


static inline int Puff_step_doBlockHeader(struct Puff_State *state, _Bool bit) {
	switch (state->bitsRead) {
		case 0:
			state->isLastBlock = bit;

			state->bitsRead++;
			break;
		case 1:
			state->compressionTypeBit0 = bit;

			state->bitsRead++;
			break;
		case 2:
			switch ((uint8_t)state->compressionTypeBit0 & (bit << 1)) {
				case 0:
					state->bitsRead = 0;
					state->id = STATE_UNCOMPRESSED;
					return Puff_step_DIRECTIVE_FINISHBYTE;
				case 1:
				case 2:
				default:
					return Puff_step_ERROR_COMPRESSIONTYPE;
			}
		default:
			return Puff_step_ERROR_BLOCKHEADER_BITSREAD;
	}
	return 0;
}
static inline int Puff_step_doUncompressed(struct Puff_State *state, uint8_t byte) {
	if (!state->lengthObtained) {
		uint16_t invertedLength = 0;
		switch (state->bytesRead) {
			case 0:
				state->uncompressed_length = byte;
				state->bytesRead++;
				break;
			case 1:
				state->uncompressed_length |= (uint16_t)byte << 8;
				state->bytesRead++;
				break;
			case 2:
				state->uncompressed_invertedLengthByte0 = byte;
				state->bytesRead++;
				break;
			case 3:
				invertedLength = ((uint16_t)byte << 8) | state->uncompressed_invertedLengthByte0;

				//printf("length: 0x%x, invertedLength: 0x%x\n", state->uncompressed_length, invertedLength);

				if ((uint16_t)~invertedLength != state->uncompressed_length) {
					return Puff_step_ERROR_NLEN;
				}

				state->lengthObtained = 1;
				state->bytesRead = 0;
				break;
			default:
				return Puff_step_ERROR_UNCOMPRESSED_LENGTH_BYTESREAD;
		}
		return 0;
	}

	Puff_iNostalgicWriter_write(&state->nostalgicWriter, byte);
	state->bytesRead++;
	//printf("bytesRead: %d out of: %d\n", state->bytesRead, state->uncompressed_length);
	if (state->bytesRead >= state->uncompressed_length) {
		
		//printf("finished reading uncommpressed data idiot.\n");
		
		if (state->isLastBlock) {
			state->id = STATE_END;
			return Puff_step_END;
		}

		state->id = STATE_BLOCKHEADER;
		state->lengthObtained = 0;
		state->bytesRead = 0;
		return 0;
	}

	return 0;
}
static inline int Puff_step_doFixed(struct Puff_State *state, _Bool bit) {
	return Puff_step_ERROR_IMPOSSIBLE;
}
static inline int Puff_step_doDynamic(struct Puff_State *state, _Bool bit) {
	return Puff_step_ERROR_IMPOSSIBLE;
}
