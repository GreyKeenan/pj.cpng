#ifndef PUFF_STATE_H
#define PUFF_STATE_H

#include "./state_impl.h"

static inline struct Puff_State Puff_State_new(struct Puff_iNostalgicWriter nostalgicWriter) {
	return (struct Puff_State) {
		.nostalgicWriter = nostalgicWriter
	};
}

#endif
