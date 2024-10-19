#ifdef DEBUG
#include <stdio.h>
#endif

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

// Headers
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// BlockHeader
// ==================================================
static inline int Puff_stepBlockHeader(struct Puff_State *state, _Bool bit);
static inline int Puff_stepBlockHeader_final(struct Puff_State *state, _Bool bit);

// Uncompressed
// ==================================================
static inline int Puff_stepUncompressed(struct Puff_State *state, uint8_t byte);
static inline int Puff_stepUncompressed_getLength(struct Puff_State *state, uint8_t byte);

// Fixed
// ==================================================
static inline int Puff_stepFixed(struct Puff_State *state, _Bool bit);
static inline int Puff_stepFixed_handleCollectedBits(struct Puff_State *state);
static inline int Puff_stepFixed_handleLeaf(struct Puff_State *state, uint16_t child);

// Dynamic
// ==================================================
static inline int Puff_stepDynamic(struct Puff_State *state, _Bool bit);

// shared fixed/dynamic
// ==================================================
int Puff_stepTree_lengthSymbol(uint16_t length, uint16_t *baseValue, uint8_t *numExtraBits);
int Puff_stepTree_distanceSymbol(uint8_t distance, uint16_t *baseValue, uint8_t *numExtraBits);
int Puff_stepTree_lz77ify(struct Puff_iNostalgicWriter *nw, uint16_t length, uint16_t distance);
static inline _Bool Puff_stepTree_collectBits(struct Puff_State_BitCollector *bc, _Bool bit);


// top function
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int Puff_step(struct Puff_State *state, uint8_t byte) {

	int e = 0;
	_Bool bit = 0;

	for (uint8_t i = 1; i != 0; i <<= 1) {

		bit = byte & i;

		switch (state->id) {
			case STATE_END:
				return Puff_step_END;
			case STATE_BLOCKHEADER:
				e = Puff_stepBlockHeader(state, bit);
				break;
			case STATE_FIXED:
				e = Puff_stepFixed(state, bit);
				break;
			case STATE_DYNAMIC:
				e = Puff_stepDynamic(state, bit);
				break;
			case STATE_UNCOMPRESSED:
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
			state->id = STATE_UNCOMPRESSED;
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
			state->id = STATE_FIXED;
			state->trees.nodeIndex = 0; //TODO ROOT
			return 0;
		case 2:
			#ifdef DEBUG
			printf("dynamic!\n");
			#endif
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
			state->id = STATE_END;
			return Puff_step_END;
		}

		state->id = STATE_BLOCKHEADER;
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

// Fixed
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

static inline int Puff_stepFixed(struct Puff_State *state, _Bool bit) {

	if (state->collector.collected < state->collector.max) {
		if (Puff_stepTree_collectBits(&state->collector, bit)) {
			return 0;
		}
		return Puff_stepFixed_handleCollectedBits(state);
	}

	int e = 0;
	uint16_t child = 0;

	e = Puff_Tree_walk(&state->trees.fixed.tree, state->trees.nodeIndex, bit, &child);
	switch (e) {
		case Puff_Tree_ISNODE:
			state->trees.nodeIndex = child;
			return 0;
		case Puff_Tree_ISLEAF:
			return Puff_stepFixed_handleLeaf(state, child);
		case Puff_Tree_OUTOFBOUNDS:
		case Puff_Tree_HALT:
		default:
			return Puff_step_ERROR_FIXED_WALK;
	}

	return Puff_step_ERROR_IMPOSSIBLE;
}
static inline int Puff_stepFixed_handleCollectedBits(struct Puff_State *state) {

	uint8_t numExtraBits = 0;

	switch (state->collector.collectFor) {
		case Puff_State_COLLECTFOR_LENGTH:
			state->lizard.length += state->collector.bits;

			state->collector = (struct Puff_State_BitCollector) {
				.max = 5,
				.collectFor = Puff_State_COLLECTFOR_FIXEDDISTANCE
			};
			return 0;

		case Puff_State_COLLECTFOR_DISTANCE:
			state->lizard.distance += state->collector.bits;

			if (Puff_stepTree_lz77ify(&state->nostalgicWriter, state->lizard.length, state->lizard.distance)) {
				return Puff_step_ERROR_FIXED_LZ77IFY;
			}
			return 0;

		case Puff_State_COLLECTFOR_FIXEDDISTANCE:
			if (Puff_stepTree_distanceSymbol(state->collector.bits, &state->lizard.distance, &numExtraBits)) {
				return Puff_step_ERROR_FIXED_MEASURE_DISTANCE;
			}
			if (numExtraBits) {
				state->collector = (struct Puff_State_BitCollector) {
					.max = numExtraBits,
					.collectFor = Puff_State_COLLECTFOR_DISTANCE
				};
				return 0;
			}

			if (Puff_stepTree_lz77ify(&state->nostalgicWriter, state->lizard.length, state->lizard.distance)) {
				return Puff_step_ERROR_FIXED_LZ77IFY;
			}
			return 0;

		default:
			return Puff_step_ERROR_FIXED_COLLECTFOR;
	}
}
static inline int Puff_stepFixed_handleLeaf(struct Puff_State *state, uint16_t child) {

	int e = 0;
	
	state->trees.nodeIndex = 0; //TODO ROOT

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

	uint8_t numExtraBits = 0;
	e = Puff_stepTree_lengthSymbol(child, &state->lizard.length, &numExtraBits);
	if (e) return Puff_step_ERROR_FIXED_MEASURE_LENGTH;

	if (numExtraBits) {
		state->collector = (struct Puff_State_BitCollector) {
			.max = numExtraBits,
			.collectFor = Puff_State_COLLECTFOR_LENGTH
		};
		return 0;
	}

	state->collector = (struct Puff_State_BitCollector) {
		.max = 5,
		.collectFor = Puff_State_COLLECTFOR_FIXEDDISTANCE
	};
	return 0;
}


// Dynamic
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

static inline int Puff_stepDynamic(struct Puff_State *state, _Bool bit) {
	return Puff_step_ERROR_IMPOSSIBLE;
}


// shared fixed/dynamic trees
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#define MIN 3
int Puff_stepTree_lengthSymbol(uint16_t length, uint16_t *baseValue, uint8_t *numExtraBits) {

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
int Puff_stepTree_distanceSymbol(uint8_t distance, uint16_t *baseValue, uint8_t *numExtraBits) {
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
int Puff_stepTree_lz77ify(struct Puff_iNostalgicWriter *nw, uint16_t length, uint16_t distance) {

	if (length < 3 || 258 < length) {
		return 1;
	}
	if (distance < 1 || 32768 < distance) {
		return 2;
	}

	int e = 0;
	uint8_t byte = 0;

	for (int i = 0; i < length; ++i) {
		e = Puff_iNostalgicWriter_nostalgize(nw, &byte, distance);
		if (e) return 3;

		e = Puff_iNostalgicWriter_write(nw, byte);
		if (e) return 4;
	}

	return 0;
}

static inline _Bool Puff_stepTree_collectBits(struct Puff_State_BitCollector *bc, _Bool bit) {

	if (bc->collectFor & 0x80) {
		bc->bits |= bit << (bc->collected);
		bc->collected++;
		return bc->collected < bc->max;
	}

	bc->bits |= bit << (bc->max - 1 - bc->collected);
	bc->collected++;
	return bc->collected < bc->max;
}
