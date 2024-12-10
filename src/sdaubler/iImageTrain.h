#ifndef SDAUBLER_IIMAGETRAIN_H
#define SDAUBLER_IIMAGETRAIN_H

#include "./iImageTrain_impl.h"

static inline int Sdaubler_iImageTrain_choochoo(struct Sdaubler_iImageTrain *self, uint32_t *nDestination) {
	return self->choochoo(self->vself, nDestination);
}
static inline int Sdaubler_iImageTrain_gauge(struct Sdaubler_iImageTrain *self, uint32_t *nWidth, uint32_t *nHeight) {
	return self->gauge(self->vself, nWidth, nHeight);
}

#endif
