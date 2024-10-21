#ifdef DEBUG
#include <stdio.h>
#endif

#include "./step.h"

#include "./error.h"
#include "./state_impl.h"

#include "./iNostalgicWriter.h"

#include "./stepTree.h"
#include "./stepFixed.h"
#include "./stepDynamic.h"

static inline int Puff_stepBlockHeader(struct Puff_State *state, _Bool bit);
static inline int Puff_stepBlockHeader_final(struct Puff_State *state, _Bool bit);

static inline int Puff_stepUncompressed(struct Puff_State *state, uint8_t byte);
static inline int Puff_stepUncompressed_getLength(struct Puff_State *state, uint8_t byte);


int Puff_step(struct Puff_State *state, uint8_t byte) {

	int e = 0;
	_Bool bit = 0;

	for (uint8_t i = 1; i != 0; i <<= 1) {

		bit = byte & i;

		switch (state->id) {
			case Puff_State_ID_END:
				return Puff_step_END;
			case Puff_State_ID_BLOCKHEADER:
				e = Puff_stepBlockHeader(state, bit);
				break;
			case Puff_State_ID_FIXED:
				e = Puff_stepFixed(state, bit);
				break;
			case Puff_State_ID_DYNAMIC:
				e = Puff_stepDynamic(state, bit);
				break;
			case Puff_State_ID_UNCOMPRESSED:
				if (i != 1) {
					return Puff_step_ERROR_UNALIGNEDUNCOMPRESSED;
				}
				return Puff_stepUncompressed(state, byte);
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

// BlockHeader
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

static inline int Puff_stepBlockHeader(struct Puff_State *state, _Bool bit) {
	switch (state->blockHeader.bitsRead) {
		case 0:
			state->isLastBlock = bit;

			state->blockHeader.bitsRead++;
			return 0;
		case 1:
			state->blockHeader.compressionTypeBit0 = bit;

			state->blockHeader.bitsRead++;
			return 0;
		case 2:
			return Puff_stepBlockHeader_final(state, bit);
		default:
			return Puff_step_ERROR_BLOCKHEADER_BITSREAD;
	}
}
static inline int Puff_stepBlockHeader_final(struct Puff_State *state, _Bool bit) {
	state->blockHeader.bitsRead = 0;
	switch ((uint8_t)state->blockHeader.compressionTypeBit0 | (bit << 1)) {
		case 0:
			#ifdef DEBUG
			printf("uncompressed!\n");
			#endif
			state->id = Puff_State_ID_UNCOMPRESSED;
			return Puff_step_DIRECTIVE_FINISHBYTE;
		case 1:
			#ifdef DEBUG
			printf("fixed!\n");
			#endif
			if (!state->fixedTreeInitiated) {
				if (Puff_FixedTree_init(&state->trees.fixed)) {
					return Puff_step_ERROR_FIXED_INIT;
				}
				state->fixedTreeInitiated = 1;
			}
			state->id = Puff_State_ID_FIXED;

			state->trees.nodeIndex = 0; //TODO ROOT
			state->collector = (struct Puff_State_BitCollector) {0};

			return 0;
		case 2:
			#ifdef DEBUG
			printf("dynamic!\n");
			#endif

			state->id = Puff_State_ID_DYNAMIC;
			state->dynamic = (struct Puff_State_Dynamic) {0};

			state->trees.nodeIndex = 0;
			state->collector = (struct Puff_State_BitCollector) {0};

			return 0;
		default:
			return Puff_step_ERROR_COMPRESSIONTYPE;
	}
}


// Uncompressed
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

static inline int Puff_stepUncompressed(struct Puff_State *state, uint8_t byte) {
	if (!state->uncompressed.lengthObtained) {
		return Puff_stepUncompressed_getLength(state, byte);
	}

	int e = Puff_iNostalgicWriter_write(&state->nostalgicWriter, byte);
	if (e) return Puff_step_ERROR_UNCOMPRESSED_WRITE;
	state->uncompressed.bytesRead++;

	if (state->uncompressed.bytesRead >= state->uncompressed.length) {
		
		if (state->isLastBlock) {
			state->id = Puff_State_ID_END;
			return Puff_step_END;
		}

		state->id = Puff_State_ID_BLOCKHEADER;
		state->uncompressed.lengthObtained = 0;
		state->uncompressed.bytesRead = 0;
		return 0;
	}

	return 0;
}
static inline int Puff_stepUncompressed_getLength(struct Puff_State *state, uint8_t byte) {
	uint16_t invertedLength = 0;
	switch (state->uncompressed.bytesRead) {
		case 0:
			state->uncompressed.length = byte;
			state->uncompressed.bytesRead++;
			break;
		case 1:
			state->uncompressed.length |= (uint16_t)byte << 8;
			state->uncompressed.bytesRead++;
			break;
		case 2:
			state->uncompressed.invertedLengthLSB = byte;
			state->uncompressed.bytesRead++;
			break;
		case 3:
			invertedLength = ((uint16_t)byte << 8) | state->uncompressed.invertedLengthLSB;

			if ((uint16_t)~invertedLength != state->uncompressed.length) {
				return Puff_step_ERROR_NLEN;
			}

			state->uncompressed.lengthObtained = 1;
			state->uncompressed.bytesRead = 0;
			break;
		default:
			return Puff_step_ERROR_UNCOMPRESSED_LENGTH_BYTESREAD;
	}
	return 0;
}
