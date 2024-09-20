#ifndef MIKE_DEFLATE_INOSTALGICWRITER_H
#define MIKE_DEFLATE_INOSTALGICWRITER_H

#include "./deflate_iNostalgicWriter_impl.h"

#define Mike_Deflate_iNostalgicWriter_TOOFAR -1

static inline int Mike_Deflate_iNostalgicWriter_write(struct Mike_Deflate_iNostalgicWriter *self, uint8_t byte) {
	return self->write(self->vself, byte);
}
static inline int Mike_Deflate_iNostalgicWriter_nostalgize(const struct Mike_Deflate_iNostalgicWriter *self, uint8_t *destination, uint32_t distanceBack) {
	return self->nostalgize(self->vself, destination, distanceBack);
}

#endif
