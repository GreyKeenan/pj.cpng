#ifndef GUNC_BitStream_H
#define GUNC_BitStream_H

#include "./iByteStream.h"

#include <stdint.h>
#include <stdbool.h>

struct Gunc_BitStream {
	struct Gunc_iByteStream bys;
	uint8_t byte;
	//	will always be the byte that the last bit was read from
	uint8_t bitpos;
	bool isMSbitFirst;
};

int Gunc_BitStream_bit(struct Gunc_BitStream *self, bool *nDestination);

static inline void Gunc_BitStream_finishByte(struct Gunc_BitStream *self) {
	self->bitpos = 0;
}
static inline int Gunc_BitStream_nextByte(struct Gunc_BitStream *self, uint8_t *nDestination) {
	Gunc_BitStream_finishByte(self);
	return Gunc_iByteStream_next(&self->bys, nDestination);
}


#endif
