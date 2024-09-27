#ifndef XYLB_STATE_H
#define XYLB_STATE_H

#include "./state_impl.h"
#include "./adleringWriter.h"
#include "./adleringWriter_impl.h"

static inline struct Xylb_State Xylb_State_new(struct Puff_iNostalgicWriter nostalgicWriter) {
	return (struct Xylb_State) {
		.adleringWriter = Xylb_AdleringWriter_new(nostalgicWriter)
	};
}

#endif
