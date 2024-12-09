#ifndef GUNC_SEQUENCE_H
#define GUNC_SEQUENCE_H

#include "gunc/log.h"
#include "gunc/iByteStream.h"

#include <stdint.h>
#include <stddef.h>

struct Gunc_Sequence {
	void *nData;
	size_t size;
	uint32_t position;
	uint32_t elements;
};

static inline int Gunc_Sequence_init(struct Gunc_Sequence *self, void *data, uint32_t elements, size_t size) {
	if (data == NULL) {
		Gunc_err("null data given");
		return 1;
	}

	*self = (struct Gunc_Sequence) {
		.nData = data,
		.size = size,
		.elements = elements
	};

	return 0;
}

int Gunc_Sequence_next_int8(void *vself, uint8_t *nDestination);

static inline int Gunc_Sequence_get(struct Gunc_Sequence *self, void **nDestination, uint32_t position) {
	if (position >= self->elements) {
		return 1;
	}
	if (self->nData == NULL) {
		Gunc_err("attempting to read from NULL nData");
		return 2;
	}

	if (nDestination != NULL) {
		*nDestination = ((uint8_t*)self->nData) + ((uint64_t)position * self->size);
		// TODO position * size overflow
	}
	
	return 0;
}

static inline int Gunc_Sequence_as_iByteStream(struct Gunc_Sequence *self, struct Gunc_iByteStream *bs) {
	if (self == NULL || bs == NULL) { //TODO checking bs is unnecessary since iByteStream_init checks it / checking self is necessary though for warning
		Gunc_err("null parameter");
		return 1;
	}

	if (self->size > 1) {
		Gunc_warn("creating iByteStream from sequence with size > 1B");
	}

	int e = Gunc_iByteStream_init(bs, self, &Gunc_Sequence_next_int8);
	if (e) {
		Gunc_err("failed to init iByteStream");
		return 2;
	}

	return 0;
}

#endif
