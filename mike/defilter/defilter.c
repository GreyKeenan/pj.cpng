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
static inline uint8_t mike_defilter_b(uint64_t i, uint8_t *scanline);

int mike_Defilter_go(mike_Ihdr ihdr, iByteTrain *bt, iByteLayer *bl) {

	int e = 0;
	uint8_t byte = 0;

	uint8_t *scanline = NULL;

	uint8_t filterType = 0;

	uint8_t samplesPerPixel = 0;
	switch (ihdr.colorType) {
		case 2:
			samplesPerPixel = 3;
			break;
		case 0:
		case 3:
			//samplesPerPixel = 1;
			//break;
		case 4:
			//samplesPerPixel = 2;
			//break;
		case 6:
			//samplesPerPixel = 4;
			//break;
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
	*/

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

		if (filterType > FILTERTYPE_MAX) {
			e = Mike_Defilter_ERROR_FILTERTYPE;
			goto finalize;
		}

		for (uint64_t i = 0; i < scanlineLength; ++i) {
			if (iByteTrain_chewchew(bt, &byte)) {
				e = Mike_Defilter_ERROR_EOTL;
				goto finalize;
			}

			//printf("initial byte: %x:\t", byte);
			switch (filterType) {
				case 0: break;
				case 1:
					byte += mike_defilter_a(i, bytesPerPixel, scanline);
					//printf("added: %x:\t", mike_defilter_a(i, bytesPerPixel, scanline));
					break;
				case 2:
					byte += mike_defilter_b(i, scanline);
					break;
				case 3:
					byte += (mike_defilter_a(i, bytesPerPixel, scanline) + mike_defilter_b(i, scanline)) >> 1;
					break;
				case 4:
					//TODO paeth
					byte = 0;
					break;
			}

			// c = scanline[i]; // this doesnt work. Need to save entire c-pixel I think? No. Just need to save the 1 corresponding byte. Im tired now though.
			scanline[i] = byte;
			iByteLayer_lay(bl, byte);
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
static inline uint8_t mike_defilter_b(uint64_t i, uint8_t *scanline) {
	return scanline[i];
}
