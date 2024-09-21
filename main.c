#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "utils/iByteTrain_forw.h"
#include "utils/iByteTrain_impl.h"
#include "utils/iByteTrain.h"

#include "mike/mike.h"
#include "mike/scanlineImage_impl.h"
#include "mike/scanlineImage_forw.h"

#include "iPixelSequence/iPixelSequence.h"
#include "iPixelSequence/iPixelSequence_impl.h"
#include "iPixelSequence/iPixelSequence_forw.h"
#include "iPixelSequence/pixel_forw.h"
#include "iPixelSequence/pixel_impl.h"

// FILE as iByteTrain
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int FILE_chewchew(void *vself, uint8_t *nDestination) {
	int byte = fgetc(vself);
	if (byte == EOF) {
		return iByteTrain_ENDOFTHELINE;
	}

	if (nDestination == NULL) {
		return 0;
	}
	*nDestination = byte;

	return 0;
}

FILE *FILE_as_iByteTrain(const char *path, iByteTrain *destination) {
	
	FILE *f = fopen(path, "rb");
	if (f == NULL) {
		return NULL;
	}

	*destination = (iByteTrain) {
		.vself = f,
		.chewchew = &FILE_chewchew
	};
	return f;

}

// scanline image to iPixelSequence
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

struct ScanlineImage_To_iPixelSequence {
	Mike_ScanlineImage image;
	uint64_t position; //by byte not by pixel
	uint8_t positionBits; //for sub-byte bitdepths
	uint32_t pixelX;
};
int ScanlineImage_To_iPixelSequence_next(void *vself, iPixelSequence_Pixel *nDestination) {
	struct ScanlineImage_To_iPixelSequence *self = vself;
	struct iPixelSequence_Pixel pixel = {0};

	return 1;
}
iPixelSequence ScanlineImage_as_iPixelSequence(struct ScanlineImage_To_iPixelSequence *self) {
	return (iPixelSequence) {
		.vself = self,
		.next = &ScanlineImage_To_iPixelSequence_next
	};
}
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int main(int argc, const char **argv) {

	struct iByteTrain bt;
	//FILE *f = FILE_as_iByteTrain("pngs/PNG_transparency_demonstration.png", &bt);
	FILE *f = FILE_as_iByteTrain("pngs/uncompressed.png", &bt);
	if (f == NULL) {
		printf("cant open file\n");
		return 1;
	}
	
	//struct Mike_ScanlineImage image = {0};
	struct ScanlineImage_To_iPixelSequence image = {0};

	int e = Mike_decode(&bt, &image.image);
	printf("\nfinal error status: 0x%x\n", e);

	fclose(f);
	bt = (struct iByteTrain){0};

	struct iPixelSequence pxs = ScanlineImage_as_iPixelSequence(&image);

	return 0;
}
