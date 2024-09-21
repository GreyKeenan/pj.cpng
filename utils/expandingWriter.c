#include "./expandingWriter.h"

#include <stdlib.h>
#include <limits.h>

int ExpandingWriter_write(void *vself, uint8_t byte) {
	struct ExpandingWriter *self = vself;
	void *tempPtr = NULL;

	if (self->writePosition >= self->cap) {

		if ((uint64_t)self->cap + self->step > UINT32_MAX) {
			return iByteLayer_LIMIT;
		}

		tempPtr = realloc(self->nData, self->cap + self->step);
		if (tempPtr == NULL) {
			return iByteLayer_RETRYABLE;
		}

		self->cap += self->step;
		self->nData = tempPtr;

		if (self->writePosition >= self->cap) { //shouldnt happen but in case | could loop instead but nah
			return iByteLayer_RETRYABLE;
		}
	}

	if (self->nData == NULL) {
		return 1;
	}
	self->nData[self->writePosition] = byte;

	if (self->writePosition != UINT32_MAX) {
		self->writePosition++;
	}

	return 0;
}
