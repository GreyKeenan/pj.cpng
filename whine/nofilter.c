#include "./nofilter.h"

#include "./image.h"

#include "gunc/log.h"
#include "gunc/iByteStream.h"
#include "gunc/iByteWriter.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

static inline int Whine_scanline( struct Gunc_iByteStream *bs, struct Gunc_iByteWriter *bw, uint8_t *scanlineBuffer, uint64_t scanlineLength, uint8_t filterType);


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
	//uint8_t bytesPerPixel = (bitsPerPixel / 8) + (bool)(bitsPerPixel % 8);
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

		e = Whine_scanline(bs, bw, hnScanline, bytesPerScanline, filterType);
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

	Gunc_TODO("hashtag nofilter");
	return r;
}

static inline int Whine_scanline(
	struct Gunc_iByteStream *bs,
	struct Gunc_iByteWriter *bw,
	uint8_t *scanlineBuffer,
	uint64_t scanlineLength,
	uint8_t filterType
) {
	int e = 0;
	uint8_t byte = 0;

	for (uint32_t i = 0; i < scanlineLength; ++i) {
		e = Gunc_iByteStream_next(bs, &byte);
		if (e) {
			Gunc_nerr(e, "failed to read byte %d", i);
			return __LINE__;
		}

		switch (filterType) {
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			default:
				Gunc_err("unrecognized filtertype: %d", filterType);
				return __LINE__;
		}
	}
	
	return 0;
}
