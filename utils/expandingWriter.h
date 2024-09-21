#ifndef EXPANDINGWRITER_H
#define EXPANDINGWRITER_H

#include "./expandingWriter_impl.h"

#include "utils/iByteLayer_impl.h"

#include <stdint.h>

static inline struct ExpandingWriter ExpandingWriter_create(uint8_t *nData, uint32_t length, uint32_t cap, uint16_t step) {
	if (step == 0) step++;
	return (struct ExpandingWriter) {
		.nData = nData,
		.writePosition = length,
		.cap = cap,
		.step = step
	};
}

int ExpandingWriter_write(void *vself, uint8_t byte);

static inline struct iByteLayer ExpandingWriter_as_iByteLayer(struct ExpandingWriter *self) {
	return (struct iByteLayer) {
		.vself = self,
		.lay = &ExpandingWriter_write
	};
}

#endif
