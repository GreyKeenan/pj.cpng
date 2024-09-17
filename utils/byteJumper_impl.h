#ifndef BYTEJUMPER_IMPL_H
#define BYTEJUMPER_IMPL_H

#include <stdint.h>

#include "iByteSeeker_impl.h"
#include "iByteSeeker_forw.h"

struct ByteJumper {
	iByteSeeker *bs;
	
	iByteSeeker_position *span_positions;
	uint16_t *span_lengths;
	uint16_t span_max;
	uint16_t span_count;
	
	uint16_t currentSpan;
	uint16_t distanceIntoSpan;
};

#endif
