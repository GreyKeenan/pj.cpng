#ifndef WHINE_IMAGE_H
#define WHINE_IMAGE_H

#include <stdint.h>

struct Whine_Image {
	uint32_t w;
	uint32_t h;
	uint8_t bitDepth;
	uint8_t colorType;
	uint8_t compressionMethod;
	uint8_t filterMethod;
	uint8_t interlaceMethod;

	uint8_t *hnData;
	uint32_t length;
};

#endif
