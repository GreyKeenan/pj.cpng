#ifndef GUNC_ByteSeq64_H
#define GUNC_ByteSeq64_H

#include "./iByteStream.h"

#include <stdint.h>

struct Gunc_ByteSeq64 {
	uint8_t *data;
	uint64_t length;
	uint64_t position;
};

int Gunc_ByteSeq64_next(struct Gunc_ByteSeq64 *self, uint8_t *nDestination);

static inline int Gunc_ByteSeq64_as_iByteStream(struct Gunc_ByteSeq64 *self, struct Gunc_iByteStream *bs) {
	return Gunc_iByteStream_init(
		bs,
		self,
		(int(*)(void*, uint8_t*))&Gunc_ByteSeq64_next
	);
}

#endif
