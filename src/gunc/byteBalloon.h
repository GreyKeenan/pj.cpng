#ifndef GUNC_ByteBalloon_H
#define GUNC_ByteBalloon_H

#include "gunc/log.h"
#include "gunc/iByteWriter.h"
#include "gunc/iByteLooker.h"

#include <stdint.h>
#include <stdlib.h>

struct Gunc_ByteBalloon {
	uint8_t *hData;
	uint32_t length;
	uint32_t cap;
};

static inline int Gunc_ByteBalloon_init(struct Gunc_ByteBalloon *self, uint32_t initialLength) {
	if (self == NULL) {
		Gunc_err("null self");
		return 1;
	}
	if (initialLength == 0) {
		Gunc_err("0 initial length.");
		return 2;
	}

	*self = (struct Gunc_ByteBalloon) {
		.hData = malloc(initialLength),
		.cap = initialLength
	};

	if (self->hData == NULL) {
		Gunc_err("malloc fail.");
		return 3;
	}

	return 0;
}

int Gunc_ByteBalloon_trim(struct Gunc_ByteBalloon *self);
/*
reallocs so that cap == length
returns 0 on success
*/

int Gunc_ByteBalloon_give(void *vself, uint8_t byte);

int Gunc_ByteBalloon_look(void *vself, uint8_t *destination, int32_t at);

static inline int Gunc_ByteBalloon_as_iByteWriter(struct Gunc_ByteBalloon *self, struct Gunc_iByteWriter *bw) {
	int e = Gunc_iByteWriter_init(bw, self, &Gunc_ByteBalloon_give);
	if (e) {
		Gunc_nerr(e, "failed to init iByteWriter.");
		return 1;
	}
	return 0;
}

static inline int Gunc_ByteBalloon_as_iByteLooker(struct Gunc_ByteBalloon *self, struct Gunc_iByteLooker *bl) {
	int e = Gunc_iByteLooker_init(bl, self, &Gunc_ByteBalloon_look);
	if (e) {
		Gunc_nerr(e, "failed to init iByteLooker.");
		return 1;
	}
	return 0;
}

#endif
