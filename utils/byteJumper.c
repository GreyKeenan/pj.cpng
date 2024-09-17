
#include "./byteJumper.h"
#include "./byteJumper_impl.h"

#include "./iByteSeeker.h"
#include "./iByteTrain.h"

#include <stdlib.h>

#define _STEP 3

void ByteJumper_init(ByteJumper *self, iByteSeeker *bs) {
	*self = (ByteJumper) {
		.bs = bs
	};
}
void ByteJumper_destroy(ByteJumper *self) {
	if (self->span_positions != NULL) {
		free(self->span_positions);
	}
	if (self->span_lengths != NULL) {
		free(self->span_lengths);
	}
}

int ByteJumper_add(ByteJumper *self, iByteSeeker_position position, uint16_t length) {
	if (self->span_count < self->span_max) {
		self->span_count++;
		self->span_positions[self->span_count] = position;
		self->span_lengths[self->span_count] = length;
		return 0;
	}

	iByteSeeker_position *new_positions = realloc(self->span_positions, sizeof(iByteSeeker_position) * (self->span_max + _STEP));
	if (new_positions == NULL) {
		return 1;
	}
	uint16_t *new_lengths = realloc(self->span_lengths, sizeof(uint16_t) * (self->span_max + _STEP));
	if (new_lengths == NULL) {
		return 1;
	}
	self->span_positions = new_positions;
	self->span_lengths = new_lengths;
	self->span_max += _STEP;

	self->span_count++;
	self->span_positions[self->span_count] = position;
	self->span_lengths[self->span_count] = length;

	return 0;
}

int ByteJumper_wind(ByteJumper *self) {
	if (self->span_count == 0) {
		return 1;
	}

	self->currentSpan = 0;
	self->distanceIntoSpan = 0;

	if (iByteSeeker_go(self->bs, self->span_positions[0])) {
		return 1;
	}

	return 0;
}

int ByteJumper_chewchew(void *vself, uint8_t *nDestination) {
	ByteJumper *self = vself;

	while (1) {
		if (self->distanceIntoSpan < self->span_lengths[self->currentSpan]) {
			break; //break if there is room in current span
		}
		if (self->currentSpan + 1 >= self->span_count) {
			return iByteTrain_ENDOFTHELINE; //EOF if no room in current span & is final span
		}
		
		if (iByteSeeker_go(self->bs, self->span_positions[self->currentSpan + 1])) {
			return 1;
		}
		self->currentSpan++;
		self->distanceIntoSpan = 0;
	}

	uint8_t byte = 0;
	switch (iByteSeeker_step(self->bs, &byte)) {
		case 0: break;
		case iByteSeeker_ERROR:
			return iByteTrain_ENDOFTHELINE;
		default:
			return 1;
	}

	if (nDestination != NULL) {
		*nDestination = byte;
	}

	return 0;
}
