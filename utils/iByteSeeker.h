#ifndef IBYTESEEKER_H
#define IBYTESEEKER_H

#include "./iByteSeeker_impl.h"

#define iByteSeeker_ERROR -1

static inline int iByteSeeker_step(struct iByteSeeker *self, uint8_t *nDestination) {
	return self->step(self->vself, nDestination);
}
static inline int iByteSeeker_at(struct iByteSeeker *self, iByteSeeker_position *nDestination) {
	return self->at(self->vself, nDestination);
}
static inline int iByteSeeker_go(struct iByteSeeker *self, iByteSeeker_position to) {
	return self->go(self->vself, to);
}

#endif
