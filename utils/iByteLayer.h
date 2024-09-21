#ifndef IBYTELAYER_H
#define IBYTELAYER_H

#include "./iByteLayer_impl.h"

static inline int iByteLayer_lay(struct iByteLayer *self, uint8_t byte) {
	return self->lay(self->vself, byte);
}

#endif
