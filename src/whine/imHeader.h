#ifndef WHINE_ImHeader_H
#define WHINE_ImHeader_H

#include "gunc/log.h"

#include <stdint.h>
#include <stdbool.h>

struct Whine_ImHeader {
	int32_t width;
	int32_t height;
	uint8_t bitDepth;
	uint8_t colorType;
	uint8_t compressionMethod;
	uint8_t filterMethod;
	uint8_t interlaceMethod;
};

#define Whine_ImHeader_BITDEPTHS_LEN 5
#define Whine_ImHeader_BITDEPTHS {1, 2, 4, 8, 16}

#define Whine_ImHeader_COLORTYPE_GREY 0
#define Whine_ImHeader_COLORTYPE_RGB 2
#define Whine_ImHeader_COLORTYPE_PALETTE 3
#define Whine_ImHeader_COLORTYPE_GREYA 4
#define Whine_ImHeader_COLORTYPE_RGBA 6

#define Whine_ImHeader_COMPRESSION_DEFAULT 0

#define Whine_ImHeader_FILTER_DEFAULT 0

#define Whine_ImHeader_INTERLACE_NONE 0
#define Whine_ImHeader_INTERLACE_ADAM7 1

#define Whine_ImHeader_SAMPLESLEN 7
extern const uint8_t Whine_ImHeader_samplesMap[Whine_ImHeader_SAMPLESLEN]; // {1, 0, 3, 1, 2, 0, 4}


static inline int8_t Whine_ImHeader_samplesPerPixel(uint8_t colorType) {
/*
from PNG spec
returns 0 if unrecognized colorType
*/
	if (colorType >= Whine_ImHeader_SAMPLESLEN) {
		return 0;
	}
	return Whine_ImHeader_samplesMap[colorType];
}

static inline uint8_t Whine_ImHeader_bitsPerPixel(const struct Whine_ImHeader *self) {
/*
returns 0 on err
*/
	uint8_t bitsPerPixel = Whine_ImHeader_samplesPerPixel(self->colorType) * self->bitDepth;
	if (bitsPerPixel > 64) {
		Gunc_err("oversized bitsPerPixel (%d) from colortype (%d) and bitDepth (%d)", bitsPerPixel, self->colorType, self->bitDepth);
		return 0;
	}
	return bitsPerPixel;
}

static inline uint8_t Whine_ImHeader_bytesPerPixel(const struct Whine_ImHeader *self) {
/*
returns 0 on err
*/
	uint8_t bitsPerPixel = Whine_ImHeader_bitsPerPixel(self);
	return (bitsPerPixel / 8) + (bool)(bitsPerPixel % 8);
}

static inline uint64_t Whine_ImHeader_bytesPerScanline(const struct Whine_ImHeader *self) {
/*
returns 0 on err
*/
	if (self->width < 1) {
		Gunc_err("undersized width: %d", self->width);
		return 0;
	}

	uint64_t bitsPerScanline = (uint64_t)Whine_ImHeader_bitsPerPixel(self) * self->width;
	return (bitsPerScanline / 8) + (bool)(bitsPerScanline % 8);
}

int Whine_ImHeader_validate(const struct Whine_ImHeader *self);
/*
ensures valid width/height
ensures that the header has a valid combination of bitDepth and colorType
ensures valid compressionMethod, filterMethod, interlaceMethod
returns 0 if valid
*/

#endif
