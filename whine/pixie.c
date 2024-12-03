#include "./pixie.h"

#include "gunc/log.h"
#include "gunc/overflow.h"


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
		}
	};

	e = Gunc_ByteSeq64_as_iByteStream(&self->seq, &self->imageStream.bys);
	if (e) {
		Gunc_nerr(e, "failed to init iByteStream");
		return 5;
	}

	return 0;
}

int Whine_Pixie_nextPixel(struct Whine_Pixie *self, uint32_t *nDestination) {
	
}
