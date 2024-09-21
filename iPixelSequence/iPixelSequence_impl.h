#ifndef IPIXELSEQUENCE_IMPL_H
#define IPIXELSEQUENCE_IMPL_H

#include "./pixel_forw.h"

#define iPixelSequence_ENDOFTHISLINE -1
#define iPixelSequence_END -2

struct iPixelSequence {
	void *vself;

	int (*next)(void *vself, iPixelSequence_Pixel *nDestination);
	/*
		gives rgba pixels sequentially
		returns 0 or (below) on success
			returns ENDOFTHISLINE *with* the final pixel of the line
			returns END *with* the final pixel of the sequence

			returns other positive values on vself-specific errors
	*/
};

#endif
