#ifndef GLASS_EXPANDINGWRITER_H
#define GLASS_EXPANDINGWRITER_H

#include "puff/iNostalgicWriter.h"
#include "utils/expandingWriter.h"

struct ExpandingWriter;

int Glass_ExpandingWriter_nostalgize(const void *vself, uint8_t *destination, uint32_t distanceBack);

static inline struct Puff_iNostalgicWriter Glass_ExpandingWriter_as_iNostalgicWriter(struct ExpandingWriter *self) {
	return (struct Puff_iNostalgicWriter) {
		.vself = self,
		.write = &ExpandingWriter_write,
		.nostalgize = &Glass_ExpandingWriter_nostalgize
	};
}

#endif
