#include "./nofilter.h"

#include "./easel.h"
#include "./filters.h"

#include "gunc/log.h"
#include "gunc/iByteStream.h"
#include "gunc/iByteWriter.h"

#include <stdlib.h>
#include <stdint.h>
#include <limits.h>

static inline int Whine_scanline(struct Gunc_iByteStream *bs, struct Gunc_iByteWriter *bw, uint8_t *scanlineBuffer, uint64_t scanlineLength, uint8_t bytesPerPixel, uint8_t filterType);
static inline int Whine_unfilterByte(uint8_t *byte, uint8_t filterType, uint32_t wi, const uint8_t *scanlineBuffer, uint64_t cBuffer, uint8_t bytesPerPixel);


int Whine_nofilter(struct Gunc_iByteStream *bs, struct Gunc_iByteWriter *bw, const struct Whine_Easel *easel) {

	if (easel->header.interlaceMethod != Whine_ImHeader_INTERLACE_NONE) {
		Gunc_err("TODO: unable to defilter interlaced images");
		return __LINE__;
	}

	int r = 0;
	int e = 0;

	uint8_t *hnScanline = NULL;


	if (easel->header.filterMethod != 0) {
		Gunc_err("unrecognized filter method: %d", easel->header.filterMethod);
		r = __LINE__;
		goto fin;
	}


	uint8_t bytesPerPixel = Whine_ImHeader_bytesPerPixel(&easel->header);
	if (bytesPerPixel == 0) {
		Gunc_nerr(bytesPerPixel, "invalid bytesPerPixel");
		r = __LINE__;
		goto fin;
	}
	uint64_t bytesPerScanline = Whine_ImHeader_bytesPerScanline(&easel->header);
	if (bytesPerScanline == 0) {
		Gunc_nerr(bytesPerScanline, "invalid bytesPerScanline");
		r = __LINE__;
		goto fin;
	}

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
	for (int32_t j = 0; j < easel->header.height; ++j) {
		e = Gunc_iByteStream_next(bs, &filterType);
		if (e) {
			Gunc_nerr(e, "failed to read filterType for line %d", j);
			r = __LINE__;
			goto fin;
		}
		#ifdef DEBUG_Whine_nofilter
		Gunc_say("line %d filter type: %d", j, filterType);
		#endif

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
