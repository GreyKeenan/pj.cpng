#include "./adleringWriter.h"

#include "puff/iNostalgicWriter.h"


static inline void Xylb_AdleringWriter_doAdler(struct Xylb_AdleringWriter *self, uint8_t byte);


struct Xylb_AdleringWriter Xylb_AdleringWriter_new(struct Puff_iNostalgicWriter nostalgicWriter) {
	return (struct Xylb_AdleringWriter) {
		.nostalgicWriter = nostalgicWriter,
		.adler.s1 = 1
	};
}

int Xylb_AdleringWriter_write(void *vself, uint8_t byte) {
	struct Xylb_AdleringWriter *self = vself;

	int e = Puff_iNostalgicWriter_write(&self->nostalgicWriter, byte);
	if (e) return e;

	Xylb_AdleringWriter_doAdler(self, byte);
	return 0;
}
int Xylb_AdleringWriter_nostalgize(const void *vself, uint8_t *destination, uint32_t distanceBack) {
	const struct Xylb_AdleringWriter *self = vself;
	return Puff_iNostalgicWriter_nostalgize(&self->nostalgicWriter, destination, distanceBack);
}


#define BASE 65521
static inline void Xylb_AdleringWriter_doAdler(struct Xylb_AdleringWriter *self, uint8_t byte) {
	self->adler.s1 = (self->adler.s1 + byte) % BASE;
	self->adler.s2 = (self->adler.s1 + self->adler.s2) % BASE;
}
