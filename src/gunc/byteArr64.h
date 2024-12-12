#ifndef GUNC_ByteArr64_H
#define GUNC_ByteArr64_H

#include "./arr.h"

#include "./iByteLooker.h"

#include <stdint.h>

int Gunc_ByteArr64_look(struct Gunc_ByteArr64 *self, uint8_t *destination, int32_t at);

static inline int Gunc_ByteArr64_as_iByteLooker(struct Gunc_ByteArr64 *self, struct Gunc_iByteLooker *bl) {
	return Gunc_iByteLooker_init(bl, self,
		(int(*)(void*, uint8_t*, int32_t))&Gunc_ByteArr64_look
	);
}

#endif
