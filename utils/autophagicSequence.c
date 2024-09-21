
#include "./autophagicSequence.h"
#include "./autophagicSequence_impl.h"
#include "./autophagicSequence_forw.h"

#include "utils/iByteTrain.h"

#include <stddef.h>

struct AutophagicSequence AutophagicSequence_create(uint8_t *data, uint32_t length, uint32_t cap) {
	return (struct AutophagicSequence) {
		.data = data,
		.length = length,
		.cap = cap
	};
}

int AutophagicSequence_read(void *vself, uint8_t *nDestination) {
	struct AutophagicSequence *self = vself;

	if (self->readPosition >= self->length) {
		return iByteTrain_ENDOFTHELINE;
	}

	if (nDestination != NULL) {
		*nDestination = self->data[self->readPosition];
	}
	self->readPosition++;

	return 0;
}

int AutophagicSequence_write(void *vself, uint8_t byte) {
	struct AutophagicSequence *self = vself;

	if (self->writePosition >= self->readPosition) {
		return 1;
	}
	if (self->writePosition >= self->cap) {
		return iByteLayer_LIMIT;
	}

	self->data[self->writePosition] = byte;
	self->writePosition++;
	if (self->writePosition > self->length) {
		self->length = self->writePosition;
	}

	return 0;
}
