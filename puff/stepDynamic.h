#ifndef PUFF_STEPDYNAMIC_H
#define PUFF_STEPDYNAMIC_H

#ifdef DEBUG
#include <stdio.h>
#endif

#include "./stepTree.h"

#include "./state_impl.h"
#include "./error.h"

//#include "./iNostalgicWriter.h"

//#include "./huffmen/tree.h"
#include "./huffmen/metaTree.h"

#include <stdint.h>

static inline int Puff_stepDynamic(struct Puff_State *state, _Bool bit);

// collecting bits
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
static inline int Puff_stepDynamic_handleCollectedBits(struct Puff_State *state);
static inline int Puff_stepDynamic_extraLength(struct Puff_State *state);
static inline int Puff_stepDynamic_extraDistance(struct Puff_State *state);
static inline int Puff_stepDynamic_bitsMeasuring(struct Puff_State *state);
static inline int Puff_stepDynamic_bitsMeta(struct Puff_State *state);

// walking trees
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
static inline int Puff_stepDynamic_meta(struct Puff_State *state, _Bool bit);
static inline int Puff_stepDynamic_meta_leaf(struct Puff_State *state, uint8_t leaf);
static inline int Puff_stepDynamic_meta_addLength(struct Puff_State_Dynamic *dynamic, uint8_t length);
/*
	when walking meta tree, writes resulting codelength values
	returns
		0 if there are more values to be written
		1 at the last value to write
		negative value on error
*/


static inline int Puff_stepDynamic(struct Puff_State *state, _Bool bit) {

	if (!state->dynamic.focus) { //TODO
		state->collector = (struct Puff_State_BitCollector) {
			.max = 5,
			.collectFor = Puff_State_COLLECTFOR_DYNAMIC_MEASURING
		};
		state->dynamic.focus = Puff_State_DYNAMIC_META;
			//TODO dont need to set this if place this check after check for collecting
	}

	if (state->collector.collected < state->collector.max) {
		#ifdef TEMP
		printf("%d", bit);
		#endif
		if (Puff_stepTree_collectBits(&state->collector, bit)) {
			return 0;
		}
		#ifdef TEMP
		printf("\n");
		#endif
		return Puff_stepDynamic_handleCollectedBits(state);
	}

	switch (state->dynamic.focus) {
		case Puff_State_DYNAMIC_META:
			return Puff_stepDynamic_meta(state, bit);
		case Puff_State_DYNAMIC_MAIN:
			// ...
		case Puff_State_DYNAMIC_DIST:
			// ...
		default:
			return Puff_step_ERROR_DYNAMIC_FOCUS;
	}

	return Puff_step_ERROR_IMPOSSIBLE;
}



// collecting bits
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

static inline int Puff_stepDynamic_handleCollectedBits(struct Puff_State *state) {

	switch (state->collector.collectFor) {

		case Puff_State_COLLECTFOR_LENGTH:
			return Puff_stepDynamic_extraLength(state);

		case Puff_State_COLLECTFOR_DISTANCE:
			return Puff_stepDynamic_extraDistance(state);

		case Puff_State_COLLECTFOR_DYNAMIC_MEASURING:
			return Puff_stepDynamic_bitsMeasuring(state);

		case Puff_State_COLLECTFOR_DYNAMIC_META:
			return Puff_stepDynamic_bitsMeta(state);

		default:
			return Puff_step_ERROR_DYNAMIC_COLLECTFOR;

	}

	return Puff_step_ERROR_IMPOSSIBLE;
}

static inline int Puff_stepDynamic_extraLength(struct Puff_State *state) {
	return Puff_step_ERROR_IMPOSSIBLE;
}
static inline int Puff_stepDynamic_extraDistance(struct Puff_State *state) {
	return Puff_step_ERROR_IMPOSSIBLE;
}
static inline int Puff_stepDynamic_bitsMeasuring(struct Puff_State *state) {
	// gets the 5b, 5b, and 4b ints counting the num of codeLengths for the trees
	
	switch (state->dynamic.unitsRead) {
		case 0:
			state->dynamic.unitsRead++;

			state->dynamic.codeLengthCount_main = 257 + state->collector.bits;
			state->collector = (struct Puff_State_BitCollector) {
				.max = 5,
				.collectFor = Puff_State_COLLECTFOR_DYNAMIC_MEASURING
			};
			return 0;
		case 1:
			state->dynamic.unitsRead++;

			state->dynamic.codeLengthCount_dist = 1 + state->collector.bits;
			state->collector = (struct Puff_State_BitCollector) {
				.max = 4,
				.collectFor = Puff_State_COLLECTFOR_DYNAMIC_MEASURING
			};
			return 0;
		case 2:
			state->dynamic.unitsRead = 0;

			state->dynamic.codeLengthCount_meta = 4 + state->collector.bits;

			#ifdef DEBUG
			printf("mainL: %d / distL: %d / metaL: %d\n",
				state->dynamic.codeLengthCount_main,
				state->dynamic.codeLengthCount_dist,
				state->dynamic.codeLengthCount_meta
			);
			#endif

			state->collector = (struct Puff_State_BitCollector) {
				.max = 3,
				.collectFor = Puff_State_COLLECTFOR_DYNAMIC_META
			};
			return 0;

		default:
			return Puff_step_ERROR_DYNAMIC_MEASURING_UNITSREAD;
	}

	return Puff_step_ERROR_IMPOSSIBLE;
}
static inline int Puff_stepDynamic_bitsMeta(struct Puff_State *state) {

	state->dynamic.lengths.meta[state->dynamic.unitsRead] = state->collector.bits;
	state->dynamic.unitsRead++;

	if (state->dynamic.unitsRead >= state->dynamic.codeLengthCount_meta) {

		state->dynamic.unitsRead = 0;

		#ifdef DEBUG
		printf("initting metaTree:\n");
		#endif

		int e = Puff_MetaTree_init(&state->trees.meta, state->dynamic.lengths.meta, state->dynamic.codeLengthCount_meta);
		#ifdef DEBUG
		printf("metaTree init status: %d\n", e);
		#endif
		if (e) {
			return Puff_step_ERROR_DYNAMIC_METATREE_INIT;
		}

		#ifdef DEBUG
		printf("walking:\n");
		#endif
		state->dynamic.focus = Puff_State_DYNAMIC_META;

		return 0;

	}

	state->collector = (struct Puff_State_BitCollector) {
		.max = 3,
		.collectFor = Puff_State_COLLECTFOR_DYNAMIC_META
	};
	return 0;
}


// walking trees
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

static inline int Puff_stepDynamic_meta(struct Puff_State *state, _Bool bit) {

	#ifdef DEBUG
	printf("%c", bit? 'r':'l');
	#endif

	uint16_t child = 0;
	int e = Puff_Tree_walk(&state->trees.meta.tree, state->trees.nodeIndex, bit, &child);
	
	switch (e) {
		case Puff_Tree_ISNODE:
			state->trees.nodeIndex = child;
			return 0;
		case Puff_Tree_ISLEAF:
			state->trees.nodeIndex = 0;
			return Puff_stepDynamic_meta_leaf(state, child);
		case Puff_Tree_OUTOFBOUNDS:
		case Puff_Tree_HALT:
		default:
			#ifdef DEBUG
			printf(" -> error: %d\n", e);
			#endif
			return Puff_step_ERROR_DYNAMIC_META_WALK;
	}

	return Puff_step_ERROR_IMPOSSIBLE;
}

static inline int Puff_stepDynamic_meta_leaf(struct Puff_State *state, uint8_t leaf) {

	int e = 0;

	#ifdef DEBUG
	printf(" -> leaf: %d\n", leaf);
	#endif

	if (leaf < 16) {
		e = Puff_stepDynamic_meta_addLength(&state->dynamic, leaf);
		#ifdef DEBUG
		printf("addLength status: %d\n", e);
		#endif
		switch (e) {
			case 0:
				return 0;
			case 1:
				// TODO ...
			default:
				return Puff_step_ERROR_DYNAMIC_META_ADDLENGTH;
		}
	}
	switch (leaf) {
		case 16:
		case 17:
		case 18:
		default:
			return Puff_step_ERROR_DYNAMIC_META_LEAF;
	}

	return Puff_step_ERROR_IMPOSSIBLE;
}

static inline int Puff_stepDynamic_meta_addLength(struct Puff_State_Dynamic *dynamic, uint8_t length) {
	
	if (dynamic->unitsRead >= Puff_LiteralTree_MAXLEAVES + Puff_DistanceTree_MAXLEAVES) {
		return -1;
	}
	if (dynamic->unitsRead >= dynamic->codeLengthCount_main + dynamic->codeLengthCount_dist) {
		return -2;
	}

	dynamic->lengths.maindist[dynamic->unitsRead] = length;
	dynamic->unitsRead++;

	if (dynamic->unitsRead >= dynamic->codeLengthCount_main + dynamic->codeLengthCount_dist) {
		dynamic->unitsRead = 0;
		return 1;
	}

	return 0;
}


#endif
