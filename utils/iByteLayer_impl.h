#ifndef IBYTELAYER_IMPL_H
#define IBYTELAYER_IMPL_H

#include <stdint.h>

struct iByteLayer {
	void *vself;

	int (*lay)(void *vself, uint8_t byte);
	/*
		writes that byte to a sequence
		returns 0 on success
	*/
};

#endif
