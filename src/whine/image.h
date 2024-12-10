#ifndef WHINE_IMAGE_H
#define WHINE_IMAGE_H

#include <stdint.h>

struct Whine_Image {
	int32_t w;
	int32_t h;
	uint8_t bitDepth;
	uint8_t colorType;
	uint8_t compressionMethod;
	uint8_t filterMethod;
	uint8_t interlaceMethod;

	uint8_t *nScanlineData;

	uint8_t *nPalette;
	int32_t paletteLength;
};

#define Whine_samplesMapLength 7
extern uint8_t Whine_samplesMap[Whine_samplesMapLength]; // {1, 0, 3, 1, 2, 0, 4}


static inline int8_t Whine_Image_samplesPerPixel(uint8_t colorType) {
/*
from PNG spec
returns 0 if unrecognized colorType
*/
	if (colorType >= Whine_samplesMapLength) {
		return 0;
	}
	return Whine_samplesMap[colorType];
}

uint8_t Whine_Image_bytesPerPixel(const struct Whine_Image *image);
	// returns 0 on error
	// always < 9
uint64_t Whine_Image_bytesPerScanline(const struct Whine_Image *image);
	// returns 0 on error
	// max is 8 * (2^31 - 1)

int Whine_Image_validateIhdr(const struct Whine_Image *image);

void Whine_Image_destroy(struct Whine_Image *self);

#endif
