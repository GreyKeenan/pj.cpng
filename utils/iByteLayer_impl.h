#ifndef IBYTELAYER_IMPL_H
#define IBYTELAYER_IMPL_H

#include <stdint.h>

#define iByteLayer_LIMIT -1
#define iByteLayer_RETRYABLE -2

struct iByteLayer {
	void *vself;

	int (*lay)(void *vself, uint8_t byte);
	/*
		writes that byte to a sequence
		returns 0 on success
			returns iByteLayer_LIMIT if failed to write & retry will not succeed either
			returns iByteLayer_RETRYABLE if failed to write, but a retry without changing any other state could succeed
			returns positive value for other vself-specific errors
	*/
};

#endif
