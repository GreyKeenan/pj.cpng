#ifndef IBYTETRAIN_IMPL_H
#define IBYTETRAIN_IMPL_H

#include <stdint.h>

struct iByteTrain {
	void *vself;

	int (*chewchew)(void *vself, uint8_t *nDestination);
};

#endif
