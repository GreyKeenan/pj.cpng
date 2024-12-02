#ifndef ZOOP_ALDERMAN_H
#define ZOOP_ALDERMAN_H

#include "gunc/iByteWriter.h"

#include <stdint.h>

struct Zoop_Alderman {
	struct Gunc_iByteWriter *bw;

	uint32_t s1;
	uint32_t s2;
};

static inline int Zoop_Alderman_init(struct Zoop_Alderman *self, struct Gunc_iByteWriter *bw) {
	if (self == NULL || bw == NULL) {
		Gunc_err("null param");
		return 1;
	}

	*self = (struct Zoop_Alderman) {
		.bw = bw,
		.s1 = 1
	};

	return 0;
}

static inline uint32_t Zoop_Alderman_total(struct Zoop_Alderman *self) {
	return (self->s2 << 16) + self->s1;
}

int Zoop_Alderman_give(void *vself, uint8_t byte);

static inline int Zoop_Alderman_as_iByteWriter(struct Zoop_Alderman *self, struct Gunc_iByteWriter *bw) {
	return Gunc_iByteWriter_init(bw, self, &Zoop_Alderman_give);
}


#endif
