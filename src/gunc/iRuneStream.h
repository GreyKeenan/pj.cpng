#ifndef GUNC_iRuneStream_H
#define GUNC_iRuneStream_H

#include "./iRune.h"

#include <stdint.h>
#include <stddef.h>

enum Gunc_iRuneStream_Status {
	  Gunc_iRuneStream_END = Gunc_iRune_END
	, Gunc_iRuneStream_RETRY = Gunc_iRune_RETRY
	, Gunc_iRuneStream_NULL = Gunc_iRune_NULL
};

struct Gunc_iRuneStream {
	void *vself;
	int (*next)(void *vself, uint32_t *nDestination);
	/*
		returns 0 on success
			returns $END when asking past sequence
			returns $RETRY when can retry without changing anything else & may succeed
			returns positive on vself-specific error
	*/
};

static inline int Gunc_iRuneStream_next(const struct Gunc_iRuneStream *self, uint32_t *nDestination) {
	if (self == NULL || self->vself == NULL || self->next == NULL) {
		return Gunc_iRuneStream_NULL;
	}
	return self->next(self->vself, nDestination);
}
static inline int Gunc_iRuneStream_next_unnull(const struct Gunc_iRuneStream *self, uint32_t *nDestination) {
	return self->next(self->vself, nDestination);
}

static inline int Gunc_iRuneStream_init(struct Gunc_iRuneStream *self, void *vself, int (*next)(void*, uint32_t*)) {
	if (self == NULL || vself == NULL || next == NULL) {
		return Gunc_iRuneStream_NULL;
	}

	*self = (struct Gunc_iRuneStream) {
		.vself = vself,
		.next = next
	};

	return 0;
}
static inline struct Gunc_iRuneStream Gunc_iRuneStream_new_unnull(void *vself, int (*next)(void*, uint32_t*)) {
	return (struct Gunc_iRuneStream) {
		.vself = vself,
		.next = next
	};
}

#endif
