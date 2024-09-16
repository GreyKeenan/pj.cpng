#ifndef IBYTETRAIN_IMPL_H
#define IBYTETRAIN_IMPL_H

#include <stdint.h>
struct iByteTrain {
	void *vself;
	int (*chewchew)(void *vself, uint8_t *nDestination);
	/*
		gives the next byte of a sequence on each call
			if destination is NULL, functions normally except doesnt give the byte back
			on err, doesnt alter *nDestination
		returns 0 on success
		returns IBYTETRAIN_ENDOFTHELINE on the next call after sending back the last byte of the sequence
		return positive value on errors
	*/
	int (*lookoutAhead)(const void *self, uint8_t *nDestination);
	/*
		gives the next byte of the sequence, without consuming it
			if dest is null, doesnt give byte
			on err, doesnt alter *nDestination
		returns 0 on success
		returns IBYTETRAIN_ENDOFTHELINE at end of sequence
		returns positive value for any other errors
	*/
};

#endif
