#include <stdio.h>

#include "./defilter.h"
#include "./error.h"

#include "mike/ihdr_impl.h"

#include "utils/iByteTrain.h"
#include "utils/iByteLayer.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define FILTERTYPE_MAX 4

static inline uint8_t mike_defilter_a(uint64_t i, uint8_t bytesPerPixel, uint8_t *scanline);
static inline uint8_t mike_defilter_c(uint64_t c, uint8_t bytesPerPixel);
static inline uint8_t mike_defilter_paeth(uint8_t a, uint8_t b, uint8_t c);

int mike_Defilter_go(mike_Ihdr ihdr, iByteTrain *bt, iByteLayer *bl) {

	int e = 0;
	uint8_t byte = 0;

	uint8_t *scanline = NULL;
	uint64_t c = 0;

	uint8_t filterType = 0;

	uint8_t samplesPerPixel = 0;
	switch (ihdr.colorType) {
		case 2:
			samplesPerPixel = 3;
			break; //TODO: only colortype 2 has been tested!
		case 0:
		case 3:
			samplesPerPixel = 1;
			break;
		case 4:
			samplesPerPixel = 2;
			break;
		case 6:
			samplesPerPixel = 4;
			break;
		default:
			e = Mike_Defilter_ERROR_COLORTYPE;
			goto finalize;
	}
	uint8_t bitsPerPixel = samplesPerPixel * ihdr.bitDepth;
	uint8_t bytesPerPixel = bitsPerPixel >> 3;
	uint64_t scanlineLengthBits = (uint64_t)bitsPerPixel * ihdr.width;
	uint64_t scanlineLength = (scanlineLengthBits >> 3) + (bool)(scanlineLengthBits & 0x07);
	//TODO in theory it could be this long, but if it is uh... there are other problems?
	/*
	printf("bytesPerPixel: %d\n", bytesPerPixel);
	printf("scanlineLengthBits: %lu, scanlineLength: %lu\n",
		scanlineLengthBits, scanlineLength
	);
	// */

	scanline = malloc(scanlineLength);
	if (scanline == NULL) {
		e = Mike_Defilter_ERROR_MALLOC;
		goto finalize;
	}
	memset(scanline, 0, scanlineLength);


	for (uint32_t j = 0; j < ihdr.height; ++j) {
		if (iByteTrain_chewchew(bt, &filterType)) {
			e = Mike_Defilter_ERROR_EOTL;
			goto finalize;
		}
		printf("filterType: %x\n", filterType);

		c = 0;
		for (uint64_t i = 0; i < scanlineLength; ++i) {
			if (iByteTrain_chewchew(bt, &byte)) {
				e = Mike_Defilter_ERROR_EOTL;
				goto finalize;
			}

			switch (filterType) {
				case 0: break;
				case 1:
					byte += mike_defilter_a(i, bytesPerPixel, scanline);
					break;
				case 2:
					byte += scanline[i];
					break;
				case 3:
					byte += (mike_defilter_a(i, bytesPerPixel, scanline) + scanline[i]) >> 1;
					break;
				case 4:
					byte += mike_defilter_paeth(
						mike_defilter_a(i, bytesPerPixel, scanline),
						scanline[i],
						mike_defilter_c(c, bytesPerPixel)
					);
					break;
				default:
					e = Mike_Defilter_ERROR_FILTERTYPE;
					goto finalize;
			}

			c = (c << 8) | scanline[i];
			scanline[i] = byte;
			e = iByteLayer_lay(bl, byte);
			if (e) goto finalize;
			//printf("laying: %x\n", byte);
		}

	}



	finalize:
	if (scanline != NULL) {
		free(scanline);
	}
	return e;
}

static inline uint8_t mike_defilter_a(uint64_t i, uint8_t bytesPerPixel, uint8_t *scanline) {
	if (bytesPerPixel) {
		if (i < bytesPerPixel) return 0;
		return scanline[i - bytesPerPixel];
	}

	if (!i) return 0;
	return scanline[i - 1];
}
static inline uint8_t mike_defilter_c(uint64_t c, uint8_t bytesPerPixel) {
	if (bytesPerPixel) {
		return (c >> (8 * (bytesPerPixel - 1))) & 0xff;
	}
	return c & 0xff;
}
static inline uint8_t mike_defilter_paeth(uint8_t a, uint8_t b, uint8_t c) {
	uint16_t p = a + b - c;
	uint16_t pa = abs(p - a);
	uint16_t pb = abs(p - b);
	uint16_t pc = abs(p - c);

	if (pa <= pb && pa <= pc) {
		return a;
	}
	if (pb <= pc) {
		return b;
	}
	return c;
}
