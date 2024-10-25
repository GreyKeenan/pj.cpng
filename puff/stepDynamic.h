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

static inline int Puff_stepDynamic_handleCollectedBits(struct Puff_State *state);

static inline int Puff_stepDynamic_extraLength(struct Puff_State *state);
static inline int Puff_stepDynamic_extraDistance(struct Puff_State *state);
static inline int Puff_stepDynamic_bitsMeasuring(struct Puff_State *state);
static inline int Puff_stepDynamic_bitsMeta(struct Puff_State *state);

static inline int Puff_stepDynamic(struct Puff_State *state, _Bool bit) {

	if (!state->dynamic.focus) { //TODO
		state->collector = (struct Puff_State_BitCollector) {
			.max = 5,
			.collectFor = Puff_State_COLLECTFOR_DYNAMIC_MEASURING
		};
		state->dynamic.focus = Puff_State_DYNAMIC_META;
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

	return Puff_step_ERROR_IMPOSSIBLE;
}

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
		if (e) {
			#ifdef DEBUG
			printf("metaTree init error: %d\n", e);
			#endif
			return Puff_State_ERROR_DYNAMIC_METATREE_INIT;
		}

		state->dynamic.focus = Puff_State_DYNAMIC_META;

		return 0;

	}

	state->collector = (struct Puff_State_BitCollector) {
		.max = 3,
		.collectFor = Puff_State_COLLECTFOR_DYNAMIC_META
	};
	return 0;
}



/*

planning out the sequence of events for stepDynamic

\/\/\/ (tree)

stepDynamic:
buildingTrees .collectingBits .walkingDistance .walkingMain
.measuring .metaTree .mainTree .distTree

measuring:
(metaTree length) (mainTree length) (distanceTree length)

metaTree:

mainTree:

distTree: distanceTree


collectingBits:
doneCollecting notDone
forExtraLength forExtraDistance | (continue collecting)
(write via lz77 thing) | (set collectingBits for extraLength)

walkingMain:
leaf node
length literal | (continue from node)
needsExtraBits doesnt | (write literal)
(set collectingBits for extraLength) | (set to walk distance)

walkingDistance:
leaf node
needsExtraBits doesnt | (continue from node)
(set collectingBits for extraDistance) | (write via lz77 thing)


/\/\/\

*/
#endif
