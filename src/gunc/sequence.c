#include "./sequence.h"

int Gunc_Sequence_next_int8(void *vself, uint8_t *nDestination) {
	struct Gunc_Sequence *self = vself;

	uint8_t *byte = NULL;

	int e = Gunc_Sequence_get(vself, (void**)&byte, self->position);
	switch (e) {
		case 0:
			break;
		case 1:
			return Gunc_iByteStream_END;
		default:
			Gunc_nerr(e, "failed to get byte");
			return 1;
	}

	self->position++;

	if (nDestination != NULL) {
		*nDestination = *byte;
	}

	return 0;
}
