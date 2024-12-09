#include "./gunc/byteSeq64.h"


int Gunc_ByteSeq64_next(struct Gunc_ByteSeq64 *self, uint8_t *nDestination) {
	if (self->position >= self->length) {
		return Gunc_iByteStream_END;
	}

	if (nDestination != NULL) {
		*nDestination = self->data[self->position];
	}
	self->position++;

	return 0;
}
