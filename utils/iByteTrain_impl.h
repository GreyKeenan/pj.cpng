#ifndef IBYTETRAIN_IMPL_H
#define IBYTETRAIN_IMPL_H

#include <stdint.h>

#define iByteTrain_ENDOFTHELINE -1
#define iByteTrain_RETRYABLE -2

struct iByteTrain {
	void *vself;

	int (*chewchew)(void *vself, uint8_t *nDestination);
	/*
		gives bytes sequentailly

		returns 0 on success
			returns ENDOFTHELINE when at the end of the sequence
			returns RETRYABLE if failed & can trying again, without modifying any other state, might succeed
			returns positive value for vself-specific error
	*/
};

#endif
