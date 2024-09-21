#ifndef IPIXELSEQUENCE_H
#define IPIXELSEQUENCE_H

#include "./iPixelSequence_impl.h"
#include "./pixel_forw.h"

static inline int iPixelSequence_next(struct iPixelSequence *self, iPixelSequence_Pixel *nDestination) {
	return self->next(self->vself, nDestination);
}

#endif
