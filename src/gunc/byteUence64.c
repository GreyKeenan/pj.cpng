#include "./gunc/byteUence64.h"


int Gunc_ByteUence64_next(struct Gunc_ByteUence64 *self, uint8_t *nDestination) {
	if (self->position >= self->arr->length) {
		return Gunc_iByteStream_END;
	}

	if (nDestination != NULL) {
		*nDestination = self->arr->data[self->position];
	}
	self->position++;

	return 0;
}
