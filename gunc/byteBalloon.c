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
	self->cap = self->length;

	return 0;
}

int Gunc_ByteBalloon_give(void *vself, uint8_t byte) {
	struct Gunc_ByteBalloon *self = vself;
	void *vptr = NULL;

	if (self->length >= self->cap) {
		Gunc_say("attempting to extend length of balloon (%p) from %d past %d.", self, self->length, self->cap);
		if (0x80000000 & self->cap) {
			Gunc_err("maxlength cannot be increased: %d", self->cap);
			return Gunc_iByteWriter_CAP;
		}

		vptr = realloc(self->hData, self->cap << 1);
		if (vptr == NULL) {
			Gunc_err("failed to realloc");
			return Gunc_iByteWriter_RETRY;
		}

		self->hData = vptr;
		self->cap <<= 1;

		if (self->length >= self->cap) {
			Gunc_err("hData still too small after expansion.");
			return 1;
		}
	}

	if (self->hData == NULL) {
		Gunc_err("null hData");
		return 2;
	}

	/*
	// impossible since uint32_max is always >= self->cap
	if (self->length == UINT32_MAX) {
		Gunc_err("length already at uint32_max");
		return Gunc_iByteWriter_CAP;
	}
	*/

	self->hData[self->length] = byte;
	self->length++;

	return 0;
}

int Gunc_ByteBalloon_look(void *vself, uint8_t *destination, int32_t at) {
	struct Gunc_ByteBalloon *self = vself;

	if (at >= 0) {
		if (at >= self->length) {
			return Gunc_iByteLooker_END;
		}
		*destination = self->hData[at];
		return 0;
	}

	if (at < ((int64_t)self->length) * -1) {
		return Gunc_iByteLooker_END;
	}
	if (destination != NULL) {
		*destination = self->hData[self->length + at];
	}
	return 0;
}
