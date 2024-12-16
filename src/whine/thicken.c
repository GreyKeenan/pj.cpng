#include "./thicken.h"

#include "./easel.h"
#include "./canvas.h"
#include "./filters.h"

#include "gunc/log.h"
#include "gunc/iByteStream.h"
#include "gunc/iByteWriter.h"
#include "gunc/byteBalloon64.h"

#include <stdlib.h>
#include <string.h>

#define PASSES 7
const uint8_t Whine_pass_xstarts[PASSES + 1] = {0, 4, 0, 2, 0, 1, 0,	0};
const uint8_t Whine_pass_ystarts[PASSES + 1] = {0, 0, 4, 0, 2, 0, 1,	0};
const uint8_t Whine_pass_xfreqs[PASSES + 1]  = {8, 8, 4, 4, 2, 2, 1,	1};
const uint8_t Whine_pass_yfreqs[PASSES + 1]  = {8, 8, 8, 4, 4, 2, 2,	1};
#define ONEPASS PASSES

static inline int Whine_defilterPass(
	uint8_t pass,
	struct Gunc_iByteStream *bs,
	struct Gunc_ByteBalloon64 *bb,

	int32_t height,
	uint8_t *scanlineBuffer,
	uint64_t scanlineLength,
	uint8_t bytesPerPixel,
	uint8_t bitsPerPixel
);
static inline int Whine_defilterScanline(
	uint8_t pass,
	struct Gunc_iByteStream *bs,
	struct Gunc_ByteBalloon64 *bb,

	uint8_t filterType,

	uint8_t *scanlineBuffer,
	uint64_t scanlineLength,
	uint8_t bytesPerPixel,
	uint8_t bitsPerPixel
);
static inline int Whine_defilterByte(
	uint8_t *byte,
	uint8_t filterType,
	uint32_t x,
	const uint8_t *scanlineBuffer,
	uint64_t cBuffer,
	uint8_t bytesPerPixel
);

static inline int Whine_writeDefilteredByte(
	struct Gunc_ByteBalloon64 *bb,
	uint8_t byte,

	uint8_t pass,
	uint64_t bytesPerScanline,
	uint8_t bytesPerPixel,
	uint8_t bitsPerPixel
);


int Whine_thicken(const struct Whine_Easel *easel, struct Whine_Canvas *canvas, struct Gunc_iByteStream *bs) {

	if (canvas->status != Whine_Canvas_ABSENT) {
		Gunc_err("destination canvas not empty");
		return __LINE__;
	}
	if (easel->header.filterMethod != Whine_ImHeader_FILTER_DEFAULT) {
		Gunc_err("Unrecognized filter method: %d", easel->header.filterMethod);
		return __LINE__;
	}
	if (easel->header.interlaceMethod > Whine_ImHeader_INTERLACE_ADAM7) {
		Gunc_err("Unrecognized interlace method: %d", easel->header.interlaceMethod);
		return __LINE__;
	}

	int e = 0;

	struct Gunc_ByteBalloon64 bb = {0};

	uint8_t *hnScanline = NULL;

	uint8_t bitsPerPixel = Whine_ImHeader_samplesPerPixel(easel->header.colorType) * easel->header.bitDepth;
	if (bitsPerPixel == 0) {
		Gunc_nerr(bitsPerPixel, "invalid bitsPerPixel");
		e = __LINE__;
		goto fin;
	}
	uint8_t bytesPerPixel = Whine_ImHeader_bytesPerPixel(&easel->header);
	if (bytesPerPixel == 0) {
		Gunc_nerr(bytesPerPixel, "invalid bytesPerPixel");
		e = __LINE__;
		goto fin;
	}
	uint64_t bytesPerScanline = Whine_ImHeader_bytesPerScanline(&easel->header);
	if (bytesPerScanline == 0) {
		Gunc_nerr(bytesPerScanline, "invalid bytesPerScanline");
		e = __LINE__;
		goto fin;
	}

	if (bytesPerScanline > SIZE_MAX) {
		Gunc_err("scanline length too long for malloc");
		e = __LINE__;
		goto fin;
	}
	hnScanline = calloc(bytesPerScanline, 1);
	if (hnScanline == NULL) {
		Gunc_err("scanline malloc failed");
		e = __LINE__;
		goto fin;
	}

	e = Gunc_ByteBalloon64_init(&bb, &canvas->image, 1024);
	if (e) {
		Gunc_nerr(e, "failed to initialize bb");
		e = __LINE__;
		goto fin;
	}


	if (easel->header.interlaceMethod == Whine_ImHeader_INTERLACE_NONE) {
		e = Whine_defilterPass(
			ONEPASS,
			bs,
			&bb,
			easel->header.height,
			hnScanline,
			bytesPerScanline,
			bytesPerPixel,
			bitsPerPixel
		);
		if (e) {
			Gunc_nerr(e, "failed to defilter non-interlaced image");
			e = __LINE__;
			goto fin;
		}
	} else {

		int32_t pixelsPerPassline = 0; // aka width of pass
		uint64_t bitsPerPassline = 0;
		uint64_t bytesPerPassline = 0;

		int32_t passlineCount = 0; // aka height of pass

		for (int i = 0; i < PASSES; ++i) {

			pixelsPerPassline =
				((easel->header.width - Whine_pass_xstarts[i]) / Whine_pass_xfreqs[i])
				+ (bool)((easel->header.width - Whine_pass_xstarts[i]) % Whine_pass_xfreqs[i]);
			bitsPerPassline = (uint64_t)Whine_ImHeader_bitsPerPixel(&easel->header) * pixelsPerPassline;
			bytesPerPassline = (bitsPerPassline / 8) + (bool)(bitsPerPassline % 8);
			passlineCount =
				((easel->header.height - Whine_pass_ystarts[i]) / Whine_pass_yfreqs[i])
				+ (bool)((easel->header.height - Whine_pass_ystarts[i]) % Whine_pass_yfreqs[i]);

			if (pixelsPerPassline == 0 || passlineCount == 0) {
				continue;
			}

			e = Whine_defilterPass(
				i,
				bs, &bb,

				passlineCount,

				hnScanline,
				bytesPerPassline,
				bytesPerPixel,
				bitsPerPixel
			);
			if (e) {
				Gunc_nerr(e, "failed to defilter pass #%d", i);
				e = __LINE__;
				goto fin;
			}
		}
	}

	e = Gunc_ByteBalloon64_trim(&bb);
	if (e) {
		Gunc_nerr(e, "failed to trim bb");
		e = __LINE__;
		goto fin;
	}
	canvas->status = Whine_Canvas_SCANLINED;

	fin:

	if (hnScanline != NULL) {
		free(hnScanline);
	}

	return e;
}



static inline int Whine_defilterPass(

	uint8_t pass,

	struct Gunc_iByteStream *bs,
	struct Gunc_ByteBalloon64 *bb,

	int32_t height,
	uint8_t *scanlineBuffer,
	uint64_t scanlineLength,
	uint8_t bytesPerPixel,
	uint8_t bitsPerPixel
) {
	int e = 0;

	memset(scanlineBuffer, 0, scanlineLength);

	uint8_t filterType = 0;
	for (int32_t i = 0; i < height; ++i) {
		e = Gunc_iByteStream_next(bs, &filterType);
		if (e) {
			Gunc_nerr(e, "failed to read filterType. pass: %d line: %d", pass, i);
			return __LINE__;
		}

		e = Whine_defilterScanline(pass, bs, bb, filterType, scanlineBuffer, scanlineLength, bytesPerPixel, bitsPerPixel);
		if (e) {
			Gunc_nerr(e, "failed to process scanline. pass: %d line: %d", pass, i);
			return __LINE__;
		}
	}

	return 0;
}

static inline int Whine_defilterScanline(
	uint8_t pass,
		//offsets for interlaced passes
		//0,0, 1,1 when not interlaced
		//pixelNum * freq + start = pixelPos to write to
	struct Gunc_iByteStream *bs,
	struct Gunc_ByteBalloon64 *bb,

	uint8_t filterType,

	uint8_t *scanlineBuffer,
	uint64_t scanlineLength,
	uint8_t bytesPerPixel,
	uint8_t bitsPerPixel
) {

	int e = 0;

	uint64_t cBuffer = 0;
	uint8_t byte = 0;

	for (uint64_t i = 0; i < scanlineLength; ++i) {
		e = Gunc_iByteStream_next(bs, &byte);
		if (e) {
			Gunc_nerr(e, "failed to read byte #%d", i);
			return __LINE__;
		}

		e = Whine_defilterByte(&byte, filterType, i, scanlineBuffer, cBuffer, bytesPerPixel);
		if (e) {
			Gunc_nerr(e, "failed to defilter byte #%d (0x%02x)", i, byte);
			return __LINE__;
		}

		cBuffer <<= 8;
		cBuffer |= scanlineBuffer[i];

		scanlineBuffer[i] = byte;

		e = Whine_writeDefilteredByte(bb, byte, pass, scanlineLength, bytesPerPixel, bitsPerPixel);
		if (e) {
			Gunc_nerr(e, "failed to write byte: %d: 0x%02x", i, byte);
			return __LINE__;
		}

	}

	return 0;
}

static inline int Whine_defilterByte(
	uint8_t *byte,
	uint8_t filterType,
	uint32_t x,
	const uint8_t *scanlineBuffer,
	uint64_t cBuffer,
	uint8_t bytesPerPixel
) {
	switch (filterType) {
		case 0:
			break;
		case 1:
			*byte += Whine_filt_a(x, scanlineBuffer, bytesPerPixel);
			break;
		case 2:
			*byte += Whine_filt_b(x, scanlineBuffer);
			break;
		case 3:
			*byte += (Whine_filt_a(x, scanlineBuffer, bytesPerPixel) + Whine_filt_b(x, scanlineBuffer)) / 2;
			break;
		case 4:
			*byte += Whine_paeth(
				Whine_filt_a(x, scanlineBuffer, bytesPerPixel),
				Whine_filt_b(x, scanlineBuffer),
				Whine_filt_c(cBuffer, bytesPerPixel)
			);
			break;
		default:
			Gunc_err("unrecognized filtertype: %d", filterType);
			return 1;
	}
	return 0;
}


static inline int Whine_writeDefilteredByte(
	struct Gunc_ByteBalloon64 *bb,
	uint8_t byte,

	uint8_t pass,
	uint64_t bytesPerScanline,
	uint8_t bytesPerPixel,
	uint8_t bitsPerPixel
) {

	int e = 0;

	//if (pass >= PASSES) {
		e = Gunc_ByteBalloon64_give(bb, byte);
		if (e) {
			Gunc_nerr(e, "failed to write byte: 0x%02x", byte);
			return __LINE__;
		}
		return 0;
	//}


	Gunc_err("TODO writing interlaced defiltered bytes");
	return 1;


	return 0;
}
