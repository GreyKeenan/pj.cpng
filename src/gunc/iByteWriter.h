#ifndef GUNC_IBYTEWRITER_H
#define GUNC_IBYTEWRITER_H

#include "./iByte.h"

#include <stdint.h>
#include <stddef.h>

enum Gunc_iByteWriter_status {
	Gunc_iByteWriter_CAP = Gunc_iByte_CAP,
	Gunc_iByteWriter_RETRY = Gunc_iByte_RETRY,
	Gunc_iByteWriter_NULL = Gunc_iByte_NULL
};

struct Gunc_iByteWriter {
	void *vself;
	int (*give)(void *vself, uint8_t byte);
	/*
		returns 0 on success
			returns $CAP when asking past sequence
			returns $RETRY when can retry without changing anything else & may succeed
			returns positive on vself-specific error
	*/
};

static inline int Gunc_iByteWriter_give(struct Gunc_iByteWriter *self, uint8_t byte) {
	if (self == NULL || self->vself == NULL || self->give == NULL) {
		return Gunc_iByteWriter_NULL;
	}
	return self->give(self->vself, byte);
}
static inline int Gunc_iByteWriter_give_unnull(struct Gunc_iByteWriter *self, uint8_t byte) {
	return self->give(self->vself, byte);
}

static inline int Gunc_iByteWriter_init(struct Gunc_iByteWriter *self, void *vself, int (*give)(void*, uint8_t)) {
	if (self == NULL || vself == NULL || give == NULL) {
		return Gunc_iByteWriter_NULL;
	}

	*self = (struct Gunc_iByteWriter) {
		.vself = vself,
		.give = give
	};

	return 0;
}
static inline struct Gunc_iByteWriter Gunc_iByteWriter_new_unnull(void *vself, int (*give)(void*, uint8_t)) {
	return (struct Gunc_iByteWriter) {
		.vself = vself,
		.give = give
	};
}


#endif
