#ifndef IBYTETRAIN_H
#define IBYTETRAIN_H

#include "./iByteTrain_impl.h"

#define iByteTrain_ENDOFTHELINE -1

static inline int iByteTrain_chewchew(struct iByteTrain *self, uint8_t *nDestination) {
	return self->chewchew(self->vself, nDestination);
}

#endif
