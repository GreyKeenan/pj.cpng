#ifndef BYTEJUMPER_H
#define BYTEJUMPER_H

#include "./byteJumper_forw.h"

#include <stdint.h>

#include "./iByteSeeker_forw.h"
#include "./iByteSeeker_impl.h"
#include "./iByteTrain_forw.h"
#include "./iByteTrain_impl.h"


void ByteJumper_init(ByteJumper *self, iByteSeeker *bs);
void ByteJumper_destroy(ByteJumper *self);

int ByteJumper_add(ByteJumper *self, iByteSeeker_position position, uint16_t length);
int ByteJumper_wind(ByteJumper *self);

int ByteJumper_chewchew(void *vself, uint8_t *nDestination);
static inline iByteTrain ByteJumper_as_iByteTrain(ByteJumper *self) {
	return (iByteTrain) {
		.vself = self,
		.chewchew = &ByteJumper_chewchew
	};
}

#endif
