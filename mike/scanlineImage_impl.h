#ifndef MIKE_SCANLINEIMAGE_IMPL_H
#define MIKE_SCANLINEIMAGE_IMPL_H

#include <stdint.h>

struct Mike_ScanlineImage {
	uint64_t length;
	uint8_t *data;
	
	uint32_t width;
	uint32_t height;

	uint8_t colorType;
	uint8_t bitDepth;
};

#endif
