#ifndef IBYTESEEKER_IMPL_H
#define IBYTESEEKER_IMPL_H

#include <stdint.h>

typedef uint32_t iByteSeeker_position;

struct iByteSeeker {
	void *vself;

	int (*step)(void *vself, uint8_t *nDestination);
	/*
		gives current byte in sequence, then advances so next byte will be returned next call
		returns
			0 on success
			iByteSeeker_ERROR at end of sequence
	*/
	int (*at)(const void *vself, iByteSeeker_position *nDestination);
	/*
		gives a unique position in the sequence
		returns
			0 on success
			iByteSeeker_ERROR if unable to yield the position
	*/
	int (*go)(void *vself, iByteSeeker_position to);
	/*
		goes to a unique position in the sequence
			the next call to look() will give the byte *at that position*, not the byte after it
		
		returns
			0 on success
			iByteSeeker_ERROR if unable to move to the given position
	*/
};

#endif
