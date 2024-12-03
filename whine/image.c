#include "./image.h"

#include <stdbool.h>

uint8_t Whine_samplesMap[Whine_samplesMapLength] = {1, 0, 3, 1, 2, 0, 4};


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
