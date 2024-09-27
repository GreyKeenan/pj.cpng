#ifndef MIKE_DECOMPRESS_H
#define MIKE_DECOMPRESS_H

#include "./state_forw.h"

#include <stdint.h>

int mike_Decompress_step(mike_Decompress_State *state, uint8_t byte);

#endif
