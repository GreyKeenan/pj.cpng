#ifndef SDAUBLER_IIMAGETRAIN_IMPL_H
#define SDAUBLER_IIMAGETRAIN_IMPL_H

#include <stdint.h>

#define Sdaubler_iImageTrain_ENDOFTHELINE -1

struct Sdaubler_iImageTrain {
	void *vself;

	int (*gauge)(const void *vself, uint32_t *nWidth, uint32_t *nHeight);
	/*
		gives width/height (in pixels)
		returns 0 on success
			returns positive value on vself-specific failure
	*/
	int (*choochoo)(void *vself, uint32_t *nDestination);
	/*
		gives the next pixel as an 8-bit RGBA value, in that order from MSB to LSB inside *nDestination
		returns 0 on success
			returns ENDOFTHELINE if requesting past image sequence
			returns positive value on vself-specific failure
	*/
};

#endif
