#include <stdio.h>

#include "./main.h"

#include "./error.h"
#include "./state_impl.h"

#include "./iNostalgicWriter.h"

#include "./huffmen/tree.h"
#include "./huffmen/fixedTree.h"

#define STATE_BLOCKHEADER 0
#define STATE_UNCOMPRESSED 1
#define STATE_FIXED 2
#define STATE_DYNAMIC 3
#define STATE_END 4

static inline int Puff_step_doBlockHeader(struct Puff_State *state, _Bool bit);
static inline int Puff_step_doUncompressed(struct Puff_State *state, uint8_t byte);
static inline int Puff_step_doFixed(struct Puff_State *state, _Bool bit);
static inline int Puff_step_doDynamic(struct Puff_State *state, _Bool bit);

int Puff_measureLengthSymbol(uint16_t length, uint16_t *baseValue, uint8_t *numExtraBits);
int Puff_measureDistanceSymbol(uint8_t distance, uint16_t *baseValue, uint8_t *numExtraBits);

int Puff_step(struct Puff_State *state, uint8_t byte) {

	int e = 0;
	_Bool bit = 0;
	//printf("[B%x]:", byte);

	for (uint8_t i = 1; i != 0; i <<= 1) {

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
			state->bitsRead = 0;
			switch ((uint8_t)state->compressionTypeBit0 | (bit << 1)) {
				case 0:
					printf("uncompressed!\n");
					state->id = STATE_UNCOMPRESSED;
					return Puff_step_DIRECTIVE_FINISHBYTE;
				case 1:
					printf("fixed!\n");
					if (!state->fixedTreeInitiated) {
						if (Puff_FixedTree_init(&state->fixedTree)) {
							return Puff_step_ERROR_FIXED_INIT;
						}
						state->fixedTreeInitiated = 1;
					}
					state->id = STATE_FIXED;
					state->currentNodeIndex = 0; //TODO ROOT
					break;
				case 2:
					printf("dynamic!\n");
				default:
					return Puff_step_ERROR_COMPRESSIONTYPE;
			}
			break;
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

	int e = Puff_iNostalgicWriter_write(&state->nostalgicWriter, byte);
	if (e) return Puff_step_ERROR_UNCOMPRESSED_WRITE;
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

	uint16_t child = 0;

	uint16_t baseLength = 0;
	uint8_t extraBits = 0;

	int e = Puff_Tree_walk(&state->fixedTree.tree, state->currentNodeIndex, bit, &child);
	switch (e) {
		case Puff_Tree_ISNODE:
			state->currentNodeIndex = child;
			printf("%d", bit);
			return 0;
		case Puff_Tree_ISLEAF:
			state->currentNodeIndex = 0; //TODO ROOT
			printf("%d) value: %d\n", bit, child);

			if (child < 256) {
				e = Puff_iNostalgicWriter_write(&state->nostalgicWriter, child);
				if (e) return Puff_step_ERROR_FIXED_WRITE;
				return 0;
			}
			if (child == 256) { // end code
				if (state->isLastBlock) {
					state->id = STATE_END;
					return Puff_step_END;
				}
				state->id = STATE_BLOCKHEADER;
				return 0;
			}
			//printf("ahh it cant handle length/distance pairs yet everybody panic\n");

			e = Puff_measureLengthSymbol(child, &baseLength, &extraBits);
			if (e) return e;

			// ...


			return 0;
		case Puff_Tree_OUTOFBOUNDS:
		case Puff_Tree_HALT:
		default:
			return Puff_step_ERROR_FIXED_WALK;
	}

	return Puff_step_ERROR_IMPOSSIBLE;
}
static inline int Puff_step_doDynamic(struct Puff_State *state, _Bool bit) {
	return Puff_step_ERROR_IMPOSSIBLE;
}


#define MIN 3
int Puff_measureLengthSymbol(uint16_t length, uint16_t *baseValue, uint8_t *numExtraBits) {

	if (length == 285) {
		*numExtraBits = 0;
		*baseValue = 258;
		return 0;
	}
	if (length < 257 || 285 < length) {
		return 1;
	}

	uint16_t l = length - 257;

	if (l < 8) {
		*numExtraBits = 0;
		*baseValue = l + MIN;
		return 0;
	}

	uint8_t df = l / 4;
	uint8_t extraBits = df - 1;
	
	*numExtraBits = extraBits;
	*baseValue = (2 << df) + (l % 4 * (1 << extraBits)) + MIN;

	return 0;
}

int Puff_measureDistanceSymbol(uint8_t distance, uint16_t *baseValue, uint8_t *numExtraBits) {
	if (distance > 29) {
		return 1;
	}

	if (distance < 4) {
		*numExtraBits = 0;
		*baseValue = distance + 1;
		return 0;
	}

	uint8_t df = distance / 2;
	uint8_t extraBits = df - 1;

	*numExtraBits = extraBits;
	*baseValue = (1 << df) + ( (distance % 2) * (1 << extraBits) ) + 1;

	return 0;
}
