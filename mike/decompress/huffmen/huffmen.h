#ifndef MIKE_DECOMPRESS_HUFFMEN_H
#define MIKE_DECOMPRESS_HUFFMEN_H

#include "mike/decompress/state_forw.h"

int mike_decompress_Huffmen_doFixed(mike_Decompress_State *state, _Bool bit);
int mike_decompress_Huffmen_doDynamic(mike_Decompress_State *state, _Bool bit);

#endif
