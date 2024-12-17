#include <stdio.h>

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
	struct Gunc_iByteStream *bs,
	struct Gunc_ByteBalloon64 *bb,

	uint8_t pass_number,
	int32_t pass_pixelsPerLine,
	int32_t pass_lines,
	uint64_t pass_bytesPerLine,
	uint8_t *pass_lineBuffer,

	uint64_t image_bytesPerLine,
	uint8_t image_bytesPerPixel,
	uint8_t image_bitsPerPixel
);
static inline int Whine_defilterScanline(
	struct Gunc_iByteStream *bs,
	struct Gunc_ByteBalloon64 *bb,

	uint8_t filterType,

	uint8_t pass_number,
	int32_t pass_pixelsPerLine,
	uint64_t pass_bytesPerLine,
	uint8_t *pass_lineBuffer,

	int32_t pass_pixelY,

	uint64_t image_bytesPerLine,
	uint8_t image_bytesPerPixel,
	uint8_t image_bitsPerPixel
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

	uint8_t pass_number,
	int32_t pass_pixelsPerLine,
	uint64_t pass_bytesPerLine,

	uint64_t pass_byteX,
	int32_t pass_pixelY,

	uint64_t image_bytesPerLine,
	uint8_t image_bytesPerPixel,
	uint8_t image_bitsPerPixel
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
			bs, &bb,

			ONEPASS,
			0, //pixelsPerPassline unnecessary for ONEPASS
			easel->header.height,

			bytesPerScanline,
			hnScanline,

			bytesPerScanline, //unnecessary too
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
				bs, &bb,

				i,
				pixelsPerPassline,
				passlineCount,

				bytesPerPassline,
				hnScanline,

				bytesPerScanline,
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
	struct Gunc_iByteStream *bs,
	struct Gunc_ByteBalloon64 *bb,

	uint8_t pass_number,
	int32_t pass_pixelsPerLine,
	int32_t pass_lines,
	uint64_t pass_bytesPerLine,
	uint8_t *pass_lineBuffer,

	uint64_t image_bytesPerLine,
	uint8_t image_bytesPerPixel,
	uint8_t image_bitsPerPixel
) {
	int e = 0;

	memset(pass_lineBuffer, 0, pass_bytesPerLine);

	uint8_t filterType = 0;
	for (int32_t i = 0; i < pass_lines; ++i) {
		e = Gunc_iByteStream_next(bs, &filterType);
		if (e) {
			Gunc_nerr(e, "failed to read filterType. pass_no: %d line: %d", pass_number, i);
			return __LINE__;
		}

		e = Whine_defilterScanline(	
			bs, bb,
			filterType,

			pass_number,
			pass_pixelsPerLine,
			pass_bytesPerLine,
			pass_lineBuffer,

			i,

			image_bytesPerLine,
			image_bytesPerPixel,
			image_bitsPerPixel
		);
		if (e) {
			Gunc_nerr(e, "failed to process scanline. pass_num: %d line: %d", pass_number, i);
			return __LINE__;
		}
	}

	return 0;
}

static inline int Whine_defilterScanline(
	struct Gunc_iByteStream *bs,
	struct Gunc_ByteBalloon64 *bb,

	uint8_t filterType,

	uint8_t pass_number,
	int32_t pass_pixelsPerLine,
	uint64_t pass_bytesPerLine,
	uint8_t *pass_lineBuffer,

	int32_t pass_pixelY,

	uint64_t image_bytesPerLine,
	uint8_t image_bytesPerPixel,
	uint8_t image_bitsPerPixel

) {

	int e = 0;

	uint64_t cBuffer = 0;
	uint8_t byte = 0;

	for (uint64_t i = 0; i < pass_bytesPerLine; ++i) {
		e = Gunc_iByteStream_next(bs, &byte);
		if (e) {
			Gunc_nerr(e, "failed to read byte #%d", i);
			return __LINE__;
		}

		e = Whine_defilterByte(&byte, filterType, i, pass_lineBuffer, cBuffer, image_bytesPerPixel);
		if (e) {
			Gunc_nerr(e, "failed to defilter byte #%d (0x%02x)", i, byte);
			return __LINE__;
		}

		cBuffer <<= 8;
		cBuffer |= pass_lineBuffer[i];

		pass_lineBuffer[i] = byte;

		e = Whine_writeDefilteredByte(
			bb, byte,

			pass_number,
			pass_pixelsPerLine,
			pass_bytesPerLine,

			i, //ERR wrong here
			pass_pixelY,

			image_bytesPerLine,
			image_bytesPerPixel,
			image_bitsPerPixel
		);
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

	uint8_t pass_number,
	int32_t pass_pixelsPerLine,
	uint64_t pass_bytesPerLine, //

	uint64_t pass_byteX,
	int32_t pass_pixelY,

	uint64_t image_bytesPerLine,
	uint8_t image_bytesPerPixel,
	uint8_t image_bitsPerPixel
) {

	int e = 0;

	if (pass_number >= PASSES) {
		e = Gunc_ByteBalloon64_give(bb, byte);
		if (e) {
			Gunc_nerr(e, "failed to write byte: 0x%02x", byte);
			return __LINE__;
		}
		return 0;
	}


	if (image_bitsPerPixel >= 8) {
		Gunc_err("TODO bitsPerPixel >= 8");
		return __LINE__;
	}



	const uint8_t mask = (1 << image_bitsPerPixel) - 1;
	const uint8_t image_pixelsPerByte = 8/image_bitsPerPixel; //assuming valid bitsPerPixel

	const int32_t image_pixelY = pass_pixelY * Whine_pass_yfreqs[pass_number] + Whine_pass_ystarts[pass_number];
	const uint64_t image_byteY = (uint64_t)image_pixelY * image_bytesPerLine;

	int32_t pass_pixelX = 0;
	uint8_t currentPixel = 0;
	int32_t image_pixelX = 0;
	uint64_t image_byteX = 0;
	uint64_t image_byteIndex = 0;

	for (int i = 0; i < image_pixelsPerByte; ++i) {
		pass_pixelX = pass_byteX * image_pixelsPerByte + i;
		if (pass_pixelX >= pass_pixelsPerLine) {
			break;
		}

		image_pixelX = pass_pixelX * Whine_pass_xfreqs[pass_number] + Whine_pass_xstarts[pass_number];
		image_byteX = (uint64_t)image_pixelX * image_bitsPerPixel / 8;
			//since bytesPerPixel is rounded up, have to use bitsPerPixel to round down
		image_byteIndex = image_byteY + image_byteX;

		currentPixel = byte >> (8 - (i + 1) * image_bitsPerPixel);
		currentPixel &= mask;

		currentPixel <<= (8 - (image_pixelX + 1) * image_bitsPerPixel) % 8;
		currentPixel |= Gunc_ByteBalloon64_get(bb, image_byteIndex);

		Gunc_say("writing pixel (0x%02x) (from 0x%02x with 0x%02x) to (%d, %d) (%02ld)",
			currentPixel,
			byte, Gunc_ByteBalloon64_get(bb, image_byteIndex),
			image_pixelX, image_pixelY, image_byteIndex
		);

		e = Gunc_ByteBalloon64_giveAt(bb, image_byteIndex, currentPixel);
		if (e) {
			Gunc_nerr(e, "failed to give byte 0x%02x at index 0x%016x", currentPixel, image_byteIndex);
			return __LINE__;
		}
	}
	
	return 0;
}
