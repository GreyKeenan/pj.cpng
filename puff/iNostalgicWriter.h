#ifndef PUFF_INOSTALGICWRITER_H
#define PUFF_INOSTALGICWRITER_H

#include "./iNostalgicWriter_impl.h"

static inline int Puff_iNostalgicWriter_write(struct Puff_iNostalgicWriter *self, uint8_t byte) {
	return self->write(self->vself, byte);
}
static inline int Puff_iNostalgicWriter_nostalgize(const struct Puff_iNostalgicWriter *self, uint8_t *destination, uint32_t distanceBack) {
	return self->nostalgize(self->vself, destination, distanceBack);
}

#endif
