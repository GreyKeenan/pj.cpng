#include "./bitStream.h"

#include "./log.h"

#include <stddef.h>

int Gunc_BitStream_bit(struct Gunc_BitStream *self, bool *nDestination) {
	int e = 0;

	if (!self->bitpos) {
		if (self->isMSbitFirst) {
			self->bitpos = 0x80;
		} else {
			self->bitpos = 0x01;
		}

		e = Gunc_iByteStream_next(&self->bys, &self->byte);
		switch (e) {
			case 0:
				break;
			case Gunc_iByteStream_END:
				return Gunc_iByteStream_END;
			default:
				Gunc_nerr(e, "failed to read byte");
				return 1;
		}
	}

	if (nDestination != NULL) {
		*nDestination = self->bitpos & self->byte;
	}
	if (self->isMSbitFirst) {
		self->bitpos >>= 1;
	} else {
		self->bitpos <<= 1;
	}

	return 0;
}
