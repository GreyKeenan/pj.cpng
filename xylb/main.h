#ifndef XYLB_MAIN_H
#define XYLB_MAIN_H

#include <stdint.h>

struct Xylb_State;

int Xylb_decompress(struct Xylb_State *state, uint8_t byte);
/*
	decodes according to zlib compressionMethod 8 / PNGs
*/

#endif
