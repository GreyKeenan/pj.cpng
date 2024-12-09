#ifndef GUNC_iByteLooker_H
#define GUNC_iByteLooker_H

#include "./iByte.h"

#include <stdint.h>
#include <stddef.h>

enum Gunc_iByteLooker_status {
	Gunc_iByteLooker_END = Gunc_iByte_END
	, Gunc_iByteLooker_RETRY = Gunc_iByte_RETRY
	, Gunc_iByteLooker_NULL = Gunc_iByte_NULL
};

struct Gunc_iByteLooker {
	void *vself;
	int (*look)(void *vself, uint8_t *destination, int32_t at);
	/*
		positive $at counts forwards from beginning,
			(0 is considered positive)
		negative $at counts backwards from next-write-position
		gives that byte to $destination
		returns 0 on success
			returns END if past lookable area in either direction
			returns RETRY if can retry
			returns positive value on vself-error
	*/
};

static inline int Gunc_iByteLooker_look(struct Gunc_iByteLooker *self, uint8_t *destination, int32_t at) {
	if (self == NULL || self->vself == NULL || self->look == NULL || destination == NULL) {
		return Gunc_iByteLooker_NULL;
	}
	return self->look(self->vself, destination, at);
}

static inline int Gunc_iByteLooker_init(struct Gunc_iByteLooker *self, void *vself, int(*look)(void*, uint8_t*, int32_t)) {
	if (self == NULL || vself == NULL || look == NULL) {
		return Gunc_iByteLooker_NULL;
	}

	*self = (struct Gunc_iByteLooker) {
		.vself = vself,
		.look = look
	};
	return 0;
}

#endif
