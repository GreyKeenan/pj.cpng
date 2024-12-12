#include "./byteBalloon64.h"

#include <stdlib.h>


int Gunc_ByteBalloon64_init(struct Gunc_ByteBalloon64 *self, struct Gunc_ByteArr64 *arr, uint64_t cap) {

	if (self == NULL || arr == NULL) {
		Gunc_err("null");
		return __LINE__;
	}

	if (!cap) {
		++cap;
	}
	if (arr->data == NULL) {
		arr->length = 0;
	}

	void *v = NULL;
	if (cap > arr->length) {
		v = realloc(arr->data, cap);
		if (v == NULL) {
			Gunc_err("failed to realloc");
			return __LINE__;
		}
		arr->data = v;
	} else {
		cap = arr->length;
	}


	*self = (struct Gunc_ByteBalloon64) {
		.arr = arr,
		.cap = cap
	};

	return 0;
}

int Gunc_ByteBalloon64_trim(struct Gunc_ByteBalloon64 *self) {

	if (self == NULL || self->arr == NULL || self->arr->data == NULL) {
		Gunc_err("null");
		return 2;
	}

	void *vptr = NULL;

	vptr = realloc(self->arr->data, self->arr->length);
	if (vptr == NULL) {
		Gunc_err("failed to realloc");
		return 1;
	}

	self->arr->data = vptr;
	self->cap = self->arr->length;

	return 0;
}

int Gunc_ByteBalloon64_give(struct Gunc_ByteBalloon64 *self, uint8_t byte) {

	if (self == NULL || self->arr == NULL || self->arr->data == NULL) {
		Gunc_err("null");
		return 2;
	}

	void *v = NULL;

	if (self->arr->length >= self->cap) {
		if (0x8000000000000000 & self->cap) {
			Gunc_err("maxlength cannot be increased: %ld", self->cap);
			return Gunc_iByteWriter_CAP;
		}

		v = realloc(self->arr->data, self->cap << 1);
		if (v == NULL) {
			Gunc_err("failed to realloc");
			return 3;
		}

		self->arr->data = v;
		self->cap <<= 1;

		if (self->arr->length >= self->cap) {
			Gunc_err("allocation still too small after expansion");
			return 4;
		}
	}

	self->arr->data[self->arr->length] = byte;
	self->arr->length++;

	return 0;
}
