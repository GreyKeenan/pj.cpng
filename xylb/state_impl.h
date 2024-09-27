#ifndef XYLB_STATE_IMPL_H
#define XYLB_STATE_IMPL_H

#include "./adleringWriter_impl.h"

#include "puff/state_impl.h"

#include <stdint.h>

struct Xylb_State {

	uint8_t id : 2;
	uint8_t bytesRead : 3;
	uint8_t cminfo;

	uint16_t windowSize;

	uint32_t target;

	struct Xylb_AdleringWriter adleringWriter;

	struct Puff_State puffState;

};

#endif
