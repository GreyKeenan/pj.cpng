#ifndef WHINE_Pixie_H
#define WHINE_Pixie_H

#include "./image.h"

#include "./easel.h"
#include "./canvas.h"

#include "gunc/byteUence64.h"

#include "gunc/byteSeq64.h"
#include "gunc/bitStream.h"

struct Whine_Pixie {
	struct Whine_Image image;
	struct Gunc_ByteSeq64 seq;
	struct Gunc_BitStream bis;

	uint64_t pixelsGiven;


	struct Whine_Easel easel;
	struct Whine_Canvas canvas;

	struct Gunc_ByteUence64 uence;
	//struct Gunc_BitStream bis;
};

/*
static inline Whine_Pixie_orient(struct Whine_Pixie *self) {
	self->uence.arr = self->canvas.arr;
	self->bis.vself = &self->uence;
}
*/

int Whine_Pixie_init(struct Whine_Pixie *self, struct Whine_Image image);
/*
	validates ihdr
	other funcs are free to assume valid ihdr
*/

int Whine_Pixie_nextPixel(struct Whine_Pixie *self, uint32_t *nDestination);

#endif
