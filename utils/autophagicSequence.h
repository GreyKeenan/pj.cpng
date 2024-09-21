#ifndef AUTOPHAGICSEQUENCE_H
#define AUTOPHAGICSEQUENCE_H

#include "utils/iByteTrain_forw.h"
#include "utils/iByteLayer_forw.h"
#include "utils/iByteTrain_impl.h"
#include "utils/iByteLayer_impl.h"

#include <stdint.h>

struct AutophagicSequence AutophagicSequence_create(uint8_t *data, uint32_t length, uint32_t cap);

int AutophagicSequence_read(void *vself, uint8_t *nDestination);
int AutophagicSequence_write(void *vself, uint8_t byte);

static inline iByteLayer AutophagicSequence_as_iByteLayer(struct AutophagicSequence *self) {
	return (iByteLayer) {
		.vself = self,
		.lay = &AutophagicSequence_write
	};
}
static inline iByteTrain AutophagicSequence_as_iByteTrain(struct AutophagicSequence *self) {
	return (iByteTrain) {
		.vself = self,
		.chewchew = &AutophagicSequence_read
	};
}

#endif
