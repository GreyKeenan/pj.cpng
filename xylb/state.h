#ifndef XYLB_STATE_H
#define XYLB_STATE_H

#include "./state_impl.h"
#include "./adleringWriter.h"
#include "./adleringWriter_impl.h"

#include "puff/iNostalgicWriter.h"
#include "puff/iNostalgicWriter_impl.h"
#include "puff/state.h"
#include "puff/state_impl.h"

static inline void Xylb_State_init(struct Xylb_State *self, struct Puff_iNostalgicWriter nostalgicWriter) {

	*self = (struct Xylb_State) {
		.adleringWriter = Xylb_AdleringWriter_new(nostalgicWriter)
	};

	struct Puff_iNostalgicWriter nw = Xylb_AdleringWriter_as_Puff_iNostalgicWriter(&self->adleringWriter); //HERE! &self when self goes out of scope.
	self->puffState = Puff_State_new(nw);

	return;
}

#endif
