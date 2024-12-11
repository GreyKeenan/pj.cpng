#ifndef GUNC_IBYTESTREAM_H
#define GUNC_IBYTESTREAM_H

#include "./iByte.h"

#include <stdint.h>
#include <stddef.h>

enum Gunc_iByteStream_Status {
	Gunc_iByteStream_END = Gunc_iByte_END,
	Gunc_iByteStream_RETRY = Gunc_iByte_RETRY,
	Gunc_iByteStream_NULL = Gunc_iByte_NULL
};

struct Gunc_iByteStream {
	void *vself;
	int (*next)(void *vself, uint8_t *nDestination);
	/*
		returns 0 on success
			returns $END when asking past sequence
			returns $RETRY when can retry without changing anything else & may succeed
			returns positive on vself-specific error
	*/
};

static inline int Gunc_iByteStream_next(const struct Gunc_iByteStream *self, uint8_t *nDestination) {
	if (self == NULL || self->vself == NULL || self->next == NULL) {
		return Gunc_iByteStream_NULL;
	}
	return self->next(self->vself, nDestination);
}
static inline int Gunc_iByteStream_next_unnull(const struct Gunc_iByteStream *self, uint8_t *nDestination) {
	return self->next(self->vself, nDestination);
}

static inline int Gunc_iByteStream_init(struct Gunc_iByteStream *self, void *vself, int (*next)(void*, uint8_t*)) {
	if (self == NULL || vself == NULL || next == NULL) {
		return Gunc_iByteStream_NULL;
	}

	*self = (struct Gunc_iByteStream) {
		.vself = vself,
		.next = next
	};

	return 0;
}
static inline struct Gunc_iByteStream Gunc_iByteStream_new_unnull(void *vself, int (*next)(void*, uint8_t*)) {
	return (struct Gunc_iByteStream) {
		.vself = vself,
		.next = next
	};
}

#endif
