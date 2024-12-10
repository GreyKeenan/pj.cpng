#include "./imHeader.h"

#include "gunc/log.h"

#include <stdbool.h>

const uint8_t Whine_ImHeader_samplesMap[Whine_ImHeader_SAMPLESLEN] = {1, 0, 3, 1, 2, 0, 4};

static inline int Whine_ImHeader_validateBDCT(uint8_t bitDepth, uint8_t colorType);


uint8_t Whine_ImHeader_bytesPerPixel(const struct Whine_ImHeader *self) {
	uint8_t bitsPerPixel = Whine_ImHeader_samplesPerPixel(self->colorType) * self->bitDepth;
	if (bitsPerPixel > 64) {
		Gunc_err("oversized bitsPerPixel (%d) from colortype (%d) and bitDepth (%d)", bitsPerPixel, self->colorType, self->bitDepth);
		return 0;
	}
	uint8_t bytesPerPixel = (bitsPerPixel / 8) + (bool)(bitsPerPixel % 8);
	return bytesPerPixel;
}
uint64_t Whine_ImHeader_bytesPerScanline(const struct Whine_ImHeader *self) {
	if (self->width < 1) {
		Gunc_err("undersized width: %d", self->width);
		return 0;
	}

	uint8_t bitsPerPixel = Whine_ImHeader_samplesPerPixel(self->colorType) * self->bitDepth;
	uint64_t bitsPerScanline = (uint64_t)bitsPerPixel * self->width;
	uint64_t bytesPerScanline = (bitsPerScanline / 8) + (bool)(bitsPerScanline % 8);

	return bytesPerScanline;
}

int Whine_ImHeader_validate(const struct Whine_ImHeader *self) {
	if (self->width < 1) {
		Gunc_err("invalid width: %d", self->width);
		return 1;
	}
	if (self->height < 1) {
		Gunc_err("invalid height: %d", self->height);
		return 2;
	}
	
	if (Whine_ImHeader_validateBDCT(self->bitDepth, self->colorType)) {
		Gunc_err("invalid bitDepth/colorType, %d / %d", self->bitDepth, self->colorType);
		return 3;
	}

	if (self->compressionMethod) {
		Gunc_err("invalid compression method: %d", self->compressionMethod);
		return 4;
	}
	if (self->filterMethod) {
		Gunc_err("invalid filter method: %d", self->filterMethod);
		return 5;
	}
	if (self->interlaceMethod > 1) {
		Gunc_err("invalid interlace method: %d", self->interlaceMethod);
		return 6;
	}

	return 0;
}
static inline int Whine_ImHeader_validateBDCT(uint8_t bitDepth, uint8_t colorType) {
	uint8_t validBitDepths[Whine_ImHeader_BITDEPTHS_LEN] = Whine_ImHeader_BITDEPTHS;

	switch (bitDepth) {
		case 1:
		case 2:
		case 4:
		case 8:
		case 16:
			break;
		default:
			Gunc_err("invalid bitDepth: %d", bitDepth);
			return 1;
	}
	switch (colorType) {
		case 0:
			break;
		case 3:
			validBitDepths[4] = 0;
			break;
		case 2:
		case 4:
		case 6:
			validBitDepths[0] = 0;
			validBitDepths[1] = 0;
			validBitDepths[2] = 0;
			break;
		default:
			Gunc_err("invalid colorType: %d", colorType);
			return 2;
	}

	for (int i = 0; i < Whine_ImHeader_BITDEPTHS_LEN; ++i) {
		if (bitDepth == validBitDepths[i]) {
			return 0;
		}
	}

	Gunc_err("invalid bitDepth colorType combo: (%d)/(%d)", bitDepth, colorType);
	return 3;
}
