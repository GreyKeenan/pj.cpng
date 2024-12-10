#include "./image.h"

#include "gunc/log.h"

#include <stdlib.h>
#include <stdbool.h>

uint8_t Whine_samplesMap[Whine_samplesMapLength] = {1, 0, 3, 1, 2, 0, 4};

int Whine_Image_validateBDCT(uint8_t bitDepth, uint8_t colorType);


uint8_t Whine_Image_bytesPerPixel(const struct Whine_Image *image) {
	uint8_t bitsPerPixel = Whine_Image_samplesPerPixel(image->colorType) * image->bitDepth;
	if (bitsPerPixel > 64) {
		Gunc_err("oversized bitsPerPixel (%d) from colortype (%d) and bitDepth (%d)", bitsPerPixel, image->colorType, image->bitDepth);
		return 0;
	}
	uint8_t bytesPerPixel = (bitsPerPixel / 8) + (bool)(bitsPerPixel % 8);
	return bytesPerPixel;
}
uint64_t Whine_Image_bytesPerScanline(const struct Whine_Image *image) {

	if (image->w < 1) {
		Gunc_err("undersized width: %d");
		return 0;
	}

	uint8_t bitsPerPixel = Whine_Image_samplesPerPixel(image->colorType) * image->bitDepth;
	uint64_t bitsPerScanline = (uint64_t)bitsPerPixel * image->w;
	uint64_t bytesPerScanline = (bitsPerScanline / 8) + (bool)(bitsPerScanline % 8);

	return bytesPerScanline;
}


int Whine_Image_validateIhdr(const struct Whine_Image *image) {
	if (image->w < 1) {
		Gunc_err("invalid width: %d", image->w);
		return 1;
	}
	if (image->h < 1) {
		Gunc_err("invalid height: %d", image->h);
		return 2;
	}
	
	if (Whine_Image_validateBDCT(image->bitDepth, image->colorType)) {
		Gunc_err("invalid bitDepth/colorType, %d / %d", image->bitDepth, image->colorType);
		return 3;
	}

	if (image->compressionMethod) {
		Gunc_err("invalid compression method: %d", image->compressionMethod);
		return 4;
	}
	if (image->filterMethod) {
		Gunc_err("invalid filter method: %d", image->filterMethod);
		return 5;
	}
	if (image->interlaceMethod > 1) {
		Gunc_err("invalid interlace method: %d", image->interlaceMethod);
		return 6;
	}

	return 0;
}

int Whine_Image_validateBDCT(uint8_t bitDepth, uint8_t colorType) {
	#define Whine_BDsLENGTH 5
	uint8_t validBitDepths[Whine_BDsLENGTH] = {1, 2, 4, 8, 16};

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

	for (int i = 0; i < Whine_BDsLENGTH; ++i) {
		if (bitDepth == validBitDepths[i]) {
			return 0;
		}
	}

	Gunc_err("invalid bitDepth colorType combo: (%d)/(%d)", bitDepth, colorType);
	return 3;
}

void Whine_Image_destroy(struct Whine_Image *self) {
	if (self->nScanlineData != NULL) {
		free(self->nScanlineData);
		self->nScanlineData = NULL;
	}
	if (self->nPalette != NULL) {
		free(self->nPalette);
		self->nPalette = NULL;
		self->paletteLength = 0;
	}
}
