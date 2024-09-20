#ifndef MIKE_DECOMPRESS_INOSTALGICWRITER_H
#define MIKE_DECOMPRESS_INOSTALGICWRITER_H

#include "./iNostalgicWriter_impl.h"

#define Mike_Decompress_iNostalgicWriter_TOOFAR -1

static inline int Mike_Decompress_iNostalgicWriter_write(struct Mike_Decompress_iNostalgicWriter *self, uint8_t byte) {
	return self->write(self->vself, byte);
}
static inline int Mike_Decompress_iNostalgicWriter_nostalgize(const struct Mike_Decompress_iNostalgicWriter *self, uint8_t *destination, uint32_t distanceBack) {
	return self->nostalgize(self->vself, destination, distanceBack);
}

#endif
