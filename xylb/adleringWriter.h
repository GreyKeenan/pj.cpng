#ifndef XYLB_ADLERINGWRITER_H
#define XYLB_ADLERINGWRITER_H

//struct Xylb_AdleringWriter;
//struct Puff_iNostalgicWriter;
#include "./adleringWriter_impl.h"
#include "puff/iNostalgicWriter_impl.h"

struct Xylb_AdleringWriter Xylb_AdleringWriter_new(struct Puff_iNostalgicWriter nostalgicWriter);

static inline uint32_t Xylb_AdleringWriter_getAdler(struct Xylb_AdleringWriter *self) {
	return (self->adler.s2 << 16) + self->adler.s1;
}

int Xylb_AdleringWriter_write(void *vself, uint8_t byte);
int Xylb_AdleringWriter_nostalgize(const void *vself, uint8_t *destination, uint32_t distanceBack);

static inline struct Puff_iNostalgicWriter Xylb_AdleringWriter_as_Puff_iNostalgicWriter(struct Xylb_AdleringWriter *self) {
	return (struct Puff_iNostalgicWriter) {
		.vself = self,
		.write = &Xylb_AdleringWriter_write,
		.nostalgize = &Xylb_AdleringWriter_nostalgize
	};
}

#endif
