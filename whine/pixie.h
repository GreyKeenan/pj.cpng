#ifndef WHINE_Pixie_H
#define WHINE_Pixie_H

#include "./image.h"

#include "gunc/byteSeq64.h"
#include "gunc/bitStream.h"

struct Whine_Pixie {
	struct Whine_Image image;
	struct Gunc_ByteSeq64 seq;
	struct Gunc_BitStream bis;
};

int Whine_Pixie_init(struct Whine_Pixie *self, struct Whine_Image image);
/*
	validates ihdr
	other funcs are free to assume valid ihdr
*/

int Whine_Pixie_nextPixel(struct Whine_Pixie *self, uint32_t *nDestination);

#endif
