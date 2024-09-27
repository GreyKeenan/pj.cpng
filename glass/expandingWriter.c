#include "./expandingWriter.h"

#include "puff/iNostalgicWriter_impl.h"
#include "utils/expandingWriter_impl.h"

#include <stddef.h>

int Glass_ExpandingWriter_nostalgize(const void *vself, uint8_t *destination, uint32_t distanceBack) {
	const struct ExpandingWriter *self = vself;

	if (distanceBack > self->writePosition) {
		return Puff_iNostalgicWriter_TOOFAR;
	}

	if (self->nData == NULL) {
		return 1;
	}
	*destination = self->nData[self->writePosition - distanceBack];
	return 0;
}
