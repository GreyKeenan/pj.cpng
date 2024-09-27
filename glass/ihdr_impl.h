#ifndef GLASS_IHDR_IMPL_H
#define GLASS_IHDR_IMPL_H

#include <stdint.h>

struct Glass_Ihdr {
	uint32_t width;
	uint32_t height;
	uint8_t bitDepth;
	uint8_t colorType;
	uint8_t compressionMethod;
	uint8_t filterMethod;
	uint8_t interlaceMethod;
};

#endif
