#include "./alderman.h"

#include "gunc/log.h"
#include "gunc/iByteWriter.h"

#define BASE 65521

int Zoop_Alderman_give(void *vself, uint8_t byte) {
	struct Zoop_Alderman *self = vself;

	int e = Gunc_iByteWriter_give(self->bw, byte);
	if (e) {
		Gunc_nerr(e, "write failed: 0x%x", byte);
		return 1;
	}

	self->s1 += byte;
	self->s1 %= BASE;
	self->s2 += self->s1;
	self->s2 %= BASE;

	return 0;
}
