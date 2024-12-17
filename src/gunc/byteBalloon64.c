#include "./byteBalloon64.h"

#include <stdlib.h>
#include <string.h>

static inline int Gunc_ByteBalloon64_fit(struct Gunc_ByteBalloon64 *self, uint64_t x);


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
		memset( //zero out the new memory
			((uint8_t*)v) + arr->length,
			0,
			cap - arr->length
		);
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
		Gunc_err("failed to realloc for length: 0x%016lx", self->arr->length);
		return 1;
	}

	self->arr->data = vptr;
	self->cap = self->arr->length;

	return 0;
}

uint8_t Gunc_ByteBalloon64_get(struct Gunc_ByteBalloon64 *self, uint64_t index) {
	if (index >= self->arr->length) {
		return 0;
	}
	return self->arr->data[index];
}
int Gunc_ByteBalloon64_giveAt(struct Gunc_ByteBalloon64 *self, uint64_t at, uint8_t byte) {

	if (self == NULL || self->arr == NULL || self->arr->data == NULL) {
		Gunc_err("null");
		return 3;
	}

	int e = 0;

	e = Gunc_ByteBalloon64_fit(self, at);
	if (e) {
		Gunc_nerr(e, "failed to fit 0x%016lx", at);
		return 2;
	}

	self->arr->data[at] = byte;
	if (self->arr->length < at + 1) {
		self->arr->length = at + 1;
	}
	
	return 0;
}

int Gunc_ByteBalloon64_give(struct Gunc_ByteBalloon64 *self, uint8_t byte) {
	return Gunc_ByteBalloon64_giveAt(self, self->arr->length, byte);
}


static inline int Gunc_ByteBalloon64_fit(struct Gunc_ByteBalloon64 *self, uint64_t x) {
	if (x < self->cap) {
		return 0;
	}

	void *v = NULL;

	uint64_t newCap = self->cap;

	while (x >= newCap) {
		if (0x8000000000000000 & newCap) {
			Gunc_err("cap cannot be increased to necessary length: 0x%016lx", x);
			return 2;
		}
		newCap <<= 1;
	}

	v = realloc(self->arr->data, newCap);
	if (v == NULL) {
		Gunc_err("failed to realloc");
		return 3;
	}
	memset( //zero out the new memory
		((uint8_t*)v) + self->cap,
		0,
		newCap - self->cap
	);

	self->arr->data = v;
	self->cap = newCap;

	return 0;
}
