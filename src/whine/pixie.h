#ifndef WHINE_Pixie_H
#define WHINE_Pixie_H

#include "./easel.h"
#include "./canvas.h"

#include "gunc/byteUence64.h"

#include "gunc/bitStream.h"

#include "gunc/iRuneStream.h"

struct Whine_Pixie {
	struct Whine_Easel easel;
	struct Whine_Canvas canvas;
	//BE CAREFUL COPYING THIS STRUCT
	struct Gunc_ByteUence64 uence;
	struct Gunc_BitStream bis;

	uint64_t pixelsGiven;
};


/*
static inline void Whine_Pixie_orient(struct Whine_Pixie *self) {
	self->uence.arr = &self->canvas.image;
	self->bis.vself = &self->uence;
}
*/

int Whine_Pixie_init(struct Whine_Pixie *self, struct Whine_Easel *easel, struct Whine_Canvas *canvas);
/*
	validates ihdr
	other funcs are free to assume valid ihdr
*/

int Whine_Pixie_nextPixel(struct Whine_Pixie *self, uint32_t *nDestination);

static inline int Whine_Pixie_as_iRuneStream(struct Whine_Pixie *self, struct Gunc_iRuneStream *rs) {
	return Gunc_iRuneStream_init(rs, self,
		(int(*)(void*, uint32_t*))&Whine_Pixie_nextPixel
	);
}

#endif
