#ifndef GUNC_ByteUence64_H
#define GUNC_ByteUence64_H

#include "./arr.h"

#include "./iByteStream.h"

#include <stdint.h>

struct Gunc_ByteUence64 {
	const struct Gunc_ByteArr64 *arr;
	uint64_t position;
};

int Gunc_ByteUence64_next(struct Gunc_ByteUence64 *self, uint8_t *nDestination);

static inline int Gunc_ByteUence64_as_iByteStream(struct Gunc_ByteUence64 *self, struct Gunc_iByteStream *bs) {
	return Gunc_iByteStream_init(
		bs,
		self,
		(int(*)(void*, uint8_t*))&Gunc_ByteUence64_next
	);
}

#endif
