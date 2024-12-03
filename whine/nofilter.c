#include "./nofilter.h"

#include "./image.h"

#include "gunc/log.h"
#include "gunc/iByteStream.h"
#include "gunc/iByteWriter.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

static inline int Whine_scanline(struct Gunc_iByteStream *bs, struct Gunc_iByteWriter *bw, uint8_t *scanlineBuffer, uint64_t scanlineLength, uint8_t bytesPerPixel, uint8_t filterType);
static inline int Whine_unfilterByte(uint8_t *byte, uint8_t filterType, uint32_t wi, const uint8_t *scanlineBuffer, uint64_t cBuffer, uint8_t bytesPerPixel);
static inline uint8_t Whine_filt_a(uint32_t wi, const uint8_t *scanlineBuffer, uint8_t bytesPerPixel);
static inline uint8_t Whine_filt_b(uint32_t wi, const uint8_t *scanlineBuffer);
static inline uint8_t Whine_filt_c(uint64_t cBuffer, uint8_t bytesPerPixel);
static inline uint8_t Whine_paeth(uint8_t a, uint8_t b, uint8_t c);


int Whine_nofilter(struct Whine_Image image, struct Gunc_iByteStream *bs, struct Gunc_iByteWriter *bw) {

	int r = 0;
	int e = 0;

	uint8_t *hnScanline = NULL;

	if (image.filterMethod != 0) {
		Gunc_err("unrecognized filter method: %d", image.filterMethod);
		r = __LINE__;
		goto fin;
	}


	int8_t samplesPerPixel = Whine_Image_samplesPerPixel(image.colorType);
	if (samplesPerPixel == 0) {
		Gunc_err("invalid color type: %d", image.colorType);
		r = __LINE__;
		goto fin;
	}

	//ensure bytesPer___ arent 0, so can be used in calloc
	if (image.bitDepth == 0) {
		Gunc_err("zero bitDepth");
		r = __LINE__;
		goto fin;
	}
	if (image.w == 0) {
		Gunc_err("zero width");
		r = __LINE__;
		goto fin;
	}
	uint8_t bitsPerPixel = samplesPerPixel * image.bitDepth;
	uint64_t bitsPerScanline = (uint64_t)bitsPerPixel * image.w;
	uint8_t bytesPerPixel = (bitsPerPixel / 8) + (bool)(bitsPerPixel % 8);
		//assumes bytesPerPixel <= 8, which is the case for valid bitDepths
	uint64_t bytesPerScanline = (bitsPerScanline / 8) + (bool)(bitsPerScanline % 8);

	if (bytesPerScanline > SIZE_MAX) {
		Gunc_err("scanline length too long for malloc");
		r = __LINE__;
		goto fin;
	}
	hnScanline = calloc(bytesPerScanline, 1);
	if (hnScanline == NULL) {
		Gunc_err("scanline malloc failed");
		r = __LINE__;
		goto fin;
	}

	uint8_t filterType = 0;
	for (uint32_t j = 0; j < image.h; ++j) {
		e = Gunc_iByteStream_next(bs, &filterType);
		if (e) {
			Gunc_nerr(e, "failed to read filterType for line %d", j);
			r = __LINE__;
			goto fin;
		}
		Gunc_say("line %d filter type: %d", j, filterType);

		e = Whine_scanline(bs, bw, hnScanline, bytesPerScanline, bytesPerPixel, filterType);
		if (e) {
			Gunc_nerr(e, "failed to process scanline %d", j);
			r = __LINE__;
			goto fin;
		}
	}


	fin:
	if (hnScanline != NULL) {
		free(hnScanline);
	}

	return r;
}

static inline int Whine_scanline(
	struct Gunc_iByteStream *bs,
	struct Gunc_iByteWriter *bw,
	uint8_t *scanlineBuffer,
	uint64_t scanlineLength,
	uint8_t bytesPerPixel,
	uint8_t filterType
) {
	int e = 0;
	uint64_t cBuffer = 0;
	uint8_t byte = 0;

	for (uint32_t i = 0; i < scanlineLength; ++i) {
		e = Gunc_iByteStream_next(bs, &byte);
		if (e) {
			Gunc_nerr(e, "failed to read byte %d", i);
			return __LINE__;
		}

		e = Whine_unfilterByte(&byte, filterType, i, scanlineBuffer, cBuffer, bytesPerPixel);
		if (e) {
			Gunc_nerr(e, "failed to unfilter byte %d: 0x%x", i, byte);
			return __LINE__;
		}

		cBuffer <<= 8;
		cBuffer |= scanlineBuffer[i];

		scanlineBuffer[i] = byte;

		e = Gunc_iByteWriter_give(bw, byte);
		if (e) {
			Gunc_nerr(e, "failed to write byte %d: 0x%x", i, byte);
			return __LINE__;
		}

	}
	
	return 0;
}

static inline int Whine_unfilterByte(
	uint8_t *byte,
	uint8_t filterType,
	uint32_t wi,
	const uint8_t *scanlineBuffer,
	uint64_t cBuffer,
	uint8_t bytesPerPixel
) {
	switch (filterType) {
		case 0:
			break;
		case 1:
			*byte += Whine_filt_a(wi, scanlineBuffer, bytesPerPixel);
			break;
		case 2:
			*byte += Whine_filt_b(wi, scanlineBuffer);
			break;
		case 3:
			*byte += (Whine_filt_a(wi, scanlineBuffer, bytesPerPixel) + Whine_filt_b(wi, scanlineBuffer)) / 2;
			break;
		case 4:
			*byte += Whine_paeth(
				Whine_filt_a(wi, scanlineBuffer, bytesPerPixel),
				Whine_filt_b(wi, scanlineBuffer),
				Whine_filt_c(cBuffer, bytesPerPixel)
			);
			break;
		default:
			Gunc_err("unrecognized filtertype: %d", filterType);
			return 1;
	}
	return 0;
}

static inline uint8_t Whine_filt_a(uint32_t wi, const uint8_t *scanlineBuffer, uint8_t bytesPerPixel) {
	if (wi < bytesPerPixel) {
		return 0;
	}
	return scanlineBuffer[wi - bytesPerPixel];
}
static inline uint8_t Whine_filt_b(uint32_t wi, const uint8_t *scanlineBuffer) {
	return scanlineBuffer[wi];
}
static inline uint8_t Whine_filt_c(uint64_t cBuffer, uint8_t bytesPerPixel) {
	cBuffer >>= 8 * (bytesPerPixel - 1);
	return cBuffer & 0xff;
}
static inline uint8_t Whine_paeth(uint8_t a, uint8_t b, uint8_t c) {
	//Gunc_TODO("this is taken from previous prototype. Why am I abs-ing unsigned values?");
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
