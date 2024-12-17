#ifndef GUNC_ByteBalloon64_H
#define GUNC_ByteBalloon64_H

#include "./arr.h"

#include "./log.h"
#include "./iByteWriter.h"

#include <stdint.h>

struct Gunc_ByteBalloon64 {
	struct Gunc_ByteArr64 *arr;
	uint64_t cap;
};

int Gunc_ByteBalloon64_init(struct Gunc_ByteBalloon64 *self, struct Gunc_ByteArr64 *arr, uint64_t cap);
/*
if (arr->data == NULL), will malloc $cap bytes
if (arr->data != NULL), assume it is already malloced.
	if cap > length, will realloc to fit cap.
	if length > cap, will raise cap to match.
*/


int Gunc_ByteBalloon64_trim(struct Gunc_ByteBalloon64 *self);
/*
reallocs so that cap == length
returns 0 on success
*/



uint8_t Gunc_ByteBalloon64_get(struct Gunc_ByteBalloon64 *self, uint64_t index);
int Gunc_ByteBalloon64_giveAt(struct Gunc_ByteBalloon64 *self, uint64_t at, uint8_t byte);
int Gunc_ByteBalloon64_give(struct Gunc_ByteBalloon64 *self, uint8_t byte);

static inline int Gunc_ByteBalloon64_as_iByteWriter(struct Gunc_ByteBalloon64 *self, struct Gunc_iByteWriter *bw) {
	return Gunc_iByteWriter_init(bw, self,
		(int(*)(void*, uint8_t))&Gunc_ByteBalloon64_give
	);
}

#endif
