#ifndef IBYTETRAIN_H
#define IBYTETRAIN_H

#include "./iByteTrain_impl.h"

#define IBYTETRAIN_ENDOFTHELINE -1

static inline int iByteTrain_chewchew(const struct iByteTrain *self, uint8_t *nDestination) {
	return self->chewchew(self->vself, nDestination);
}
static inline int iByteTrain_lookoutAhead(const struct iByteTrain *self, uint8_t *nDestination) {
	return self->lookoutAhead(self->vself, nDestination);
}

#endif
