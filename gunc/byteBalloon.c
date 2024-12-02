#include "./byteBalloon.h"

#include <stdlib.h>
#include <limits.h>


int Gunc_ByteBalloon_trim(struct Gunc_ByteBalloon *self) {
	void *vptr = NULL;

	vptr = realloc(self->hData, self->length);
	if (vptr == NULL) {
		Gunc_err("failed to realloc");
		return 1;
	}
	
	self->hData = vptr;
	self->currentAllocatedMax = self->length;

	return 0;
}

int Gunc_ByteBalloon_give(void *vself, uint8_t byte) {
	struct Gunc_ByteBalloon *self = vself;
	void *vptr = NULL;

	if (self->length >= self->currentAllocatedMax) {
		Gunc_say("attempting to extend length of balloon (%p) from %d past %d.", self, self->length, self->currentAllocatedMax);
		if (0x80000000 & self->currentAllocatedMax) {
			Gunc_err("maxlength cannot be increased: %d", self->currentAllocatedMax);
			return Gunc_iByteWriter_CAP;
		}

		vptr = realloc(self->hData, self->currentAllocatedMax << 1);
		if (vptr == NULL) {
			Gunc_err("failed to realloc");
			return Gunc_iByteWriter_RETRY;
		}

		self->hData = vptr;
		self->currentAllocatedMax <<= 1;

		if (self->length >= self->currentAllocatedMax) {
			Gunc_err("hData still too small after expansion.");
			return 1;
		}
	}

	if (self->hData == NULL) {
		Gunc_err("null hData");
		return 2;
	}

	/*
	// impossible since uint32_max is always >= self->currentAllocatedMax
	if (self->length == UINT32_MAX) {
		Gunc_err("length already at uint32_max");
		return Gunc_iByteWriter_CAP;
	}
	*/

	self->hData[self->length] = byte;
	self->length++;

	return 0;
}
