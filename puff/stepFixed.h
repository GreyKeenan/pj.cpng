#ifndef PUFF_STEPFIXED_H
#define PUFF_STEPFIXED_H

#include "./stepTree.h"

#include "./state_impl.h"
#include "./error.h"

#include "./iNostalgicWriter.h"

#include "./huffmen/tree.h"
#include "./huffmen/fixedTree.h"

#include <stdint.h>

static inline int Puff_stepFixed(struct Puff_State *state, _Bool bit);
static inline int Puff_stepFixed_handleCollectedBits(struct Puff_State *state);
static inline int Puff_stepFixed_handleLeaf(struct Puff_State *state, uint16_t child);

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
			state->id = Puff_State_ID_END;
			return Puff_step_END;
		}
		state->id = Puff_State_ID_BLOCKHEADER;
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

#endif
