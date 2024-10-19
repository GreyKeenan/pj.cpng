#ifndef PUFF_STEPDYNAMIC_H
#define PUFF_STEPDYNAMIC_H

#include "./stepTree.h"

#include "./state_impl.h"
#include "./error.h"

#include "./iNostalgicWriter.h"

#include "./huffmen/tree.h"
#include "./huffmen/fixedTree.h"

#include <stdint.h>

//static inline int Puff_stepDynamic(struct Puff_State *state, _Bool bit);

static inline int Puff_stepDynamic(struct Puff_State *state, _Bool bit) {
	return Puff_step_ERROR_IMPOSSIBLE;
}

#endif
