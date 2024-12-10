#include "./pixie.h"

#include "gunc/log.h"
#include "gunc/overflow.h"

#include <stddef.h>

#define PLTE_ENTRY 3

static inline int Whine_Pixie_onesample(struct Gunc_BitStream *bis, uint8_t bitsPerSample, uint32_t *pixel);


int Whine_Pixie_init(struct Whine_Pixie *self, struct Whine_Image image) {

	int e = 0;

	e = Whine_Image_validateIhdr(&image);
	if (e) {
		Gunc_nerr(e, "invalid ihdr data");
		return 1;
	}
	if (image.nScanlineData == NULL) {
		Gunc_err("NULL scanline data");
		return 2;
	}

	uint64_t bytesPerScanline = Whine_Image_bytesPerScanline(&image);
	if (!bytesPerScanline) {
		Gunc_err("0 bytes per scanline");
		return 3;
	}
	if (Gunc_uCantMultiply(bytesPerScanline, image.h, UINT64_MAX)) {
		// would have failed sooner than this
		Gunc_err("image size too large to create uint64-length");
		return 4;
	}

	*self = (struct Whine_Pixie) {
		.image = image,
		.seq = (struct Gunc_ByteSeq64) {
			.data = image.nScanlineData,
			.length = image.h * bytesPerScanline
		},
		.bis = (struct Gunc_BitStream) { .isMSbitFirst = true }
	};

	e = Gunc_ByteSeq64_as_iByteStream(&self->seq, &self->bis.bys);
	if (e) {
		Gunc_nerr(e, "failed to init iByteStream");
		return 5;
	}

	return 0;
}

int Whine_Pixie_nextPixel(struct Whine_Pixie *self, uint32_t *nDestination) {
	int e = 0;
	uint32_t pixel = 0;
	uint8_t byte = 0;

	uint8_t samplesPerPixel = Whine_Image_samplesPerPixel(self->image.colorType);
	if (samplesPerPixel == 0) {
		Gunc_err("0 samples per pixel");
		return __LINE__;
	}

	for (int i = 0; i < samplesPerPixel; ++i) {
		e = Whine_Pixie_onesample(&self->bis, self->image.bitDepth, &pixel);
		if (e) {
			Gunc_nerr(e, "failed to read sample.");
			Gunc_TODO("handle iByte_END");
			return __LINE__;
		}
	}

	uint8_t paletteIndex = 0;

	switch (self->image.colorType) {
		case 0: //g
			//TODO transparent colors
			byte = pixel;
			pixel <<= 8;
			pixel |= byte;
			pixel <<= 8;
			pixel |= byte;

			pixel <<= 8;
			pixel |= 0xff;
			break;
		case 2: //rgb
			//TODO transparent colors
			pixel <<= 8;
			pixel |= 0xff;
			break;
		case 3: //i
			//TODO transparent colors
			paletteIndex = pixel & ((1 << self->image.bitDepth) - 1);
				//have to mask away since onesample() duplicates bits to fill byte
			pixel = 0;

			if (self->image.nPalette == NULL) {
				Gunc_err("palette missing");
				return __LINE__;
			}
			if (paletteIndex * PLTE_ENTRY + (PLTE_ENTRY - 1) >= self->image.paletteLength) {
				Gunc_err("palette index too large (%d >= %d)", paletteIndex, self->image.paletteLength);
				return __LINE__;
			}
			for (int i = 0; i < PLTE_ENTRY; ++i) {
				pixel |= self->image.nPalette[paletteIndex * 3 + i];
				pixel <<= 8;
			}
			pixel |= 0xff;

			break;
		case 4: //ga
			byte = pixel & 0xff;

			pixel &= 0xff00;
			pixel |= ((pixel >> 8) & 0xff);
			pixel <<= 8;
			pixel |= ((pixel >> 8) & 0xff);
			pixel <<= 8;
			pixel |= byte;
			break;
		case 6: //rgba
			break;
		default:
			Gunc_err("unrecognized colorType: %d", self->image.colorType);
			return __LINE__;
	}

	if (nDestination != NULL) {
		*nDestination = pixel;
	}

	return 0;
}

static inline int Whine_Pixie_onesample(struct Gunc_BitStream *bis, uint8_t bitsPerSample, uint32_t *pixel) {

	int e = 0;
	uint8_t byte = 0;
	bool bit = 0;

	switch (bitsPerSample) {
		case 8:
			e = Gunc_BitStream_nextByte(bis, &byte);
			if (e) {
				Gunc_nerr(e, "failed to read byte");
				return __LINE__;
			}
			*pixel <<= 8;
			*pixel |= byte;
			return 0;
		case 16:
			e = Gunc_BitStream_nextByte(bis, &byte);
			if (e) {
				Gunc_nerr(e, "failed to read MSB");
				return __LINE__;
			}
			e = Gunc_BitStream_nextByte(bis, NULL);
			if (e) {
				Gunc_nerr(e, "failed to read LSB");
				return __LINE__;
			}
			*pixel <<= 8;
			*pixel |= byte;
			return 0;
		case 1:
		case 2:
		case 4:
			break;
		default:
			Gunc_err("unable to handle (%d) bits per sample", bitsPerSample);
			return __LINE__;
	}

	for (int i = 0; i < bitsPerSample; ++i) {
		e = Gunc_BitStream_bit(bis, &bit);
		if (e) {
			Gunc_nerr(e, "bit %d read failed", i);
			return __LINE__;
		}

		byte <<= 1;
		byte |= bit;
	}

	*pixel = byte;
	for (int i = 0; i < 8 / bitsPerSample - 1; ++i) {
		*pixel <<= bitsPerSample;
		*pixel |= byte;
	}

	return 0;
}

// linear scale from PNG spec:
//	floor((input * MAXOUTSAMPLE / MAXINSAMPLE) + 0.5)
