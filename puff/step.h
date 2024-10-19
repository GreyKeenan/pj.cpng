#ifndef PUFF_MAIN_H
#define PUFF_MAIN_H

#include <stdint.h>

struct Puff_State;

int Puff_step(struct Puff_State *state, uint8_t byte);

#endif
