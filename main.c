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
	uint32_t pixelPositionX;
	/*
	uint8_t positionBits; //for sub-byte bitdepths
	uint32_t pixelX;
	*/
};
int ScanlineImage_To_iPixelSequence_chewchew(void *vself, uint8_t *nDestination) {
	struct ScanlineImage_To_iPixelSequence *self = vself;

	if (self->position >= self->image.length) {
		return iByteTrain_ENDOFTHELINE;
	}

	if (nDestination != NULL) {
		*nDestination = self->image.data[self->position];
	}
	self->position++;
	
	return 0;
}
static inline int ScanlineImage_To_iPixelSequence_chewchew16as8(void *vself, uint8_t *nDestination) {
	struct ScanlineImage_To_iPixelSequence *self = vself;

	int e = 0;
	uint8_t n = 0;
	uint8_t byte = 0;

	e = ScanlineImage_To_iPixelSequence_chewchew(self, &byte);
	if (e) return e;
	for (int i = 0x80; i != 0; i = i >> 2) {
		n = (n << 1) & (_Bool)(byte & i);
	}
	e = ScanlineImage_To_iPixelSequence_chewchew(self, &byte);
	if (e) return e;
	for (int i = 0x80; i != 0; i = i >> 2) {
		n = (n << 1) & (_Bool)(byte & i);
	}

	if (nDestination != NULL) {
		*nDestination = n;
	}

	return 0;
}
int ScanlineImage_To_iPixelSequence_next(void *vself, iPixelSequence_Pixel *nDestination) {
	struct ScanlineImage_To_iPixelSequence *self = vself;
	struct iPixelSequence_Pixel pixel = {0};

	if (self->position >= self->image.length) {
		return iPixelSequence_END;
	}

	switch (self->image.colorType) {
		case 2: //rgb
			switch (self->image.bitDepth) {
				case 8:
					if (ScanlineImage_To_iPixelSequence_chewchew(self, &pixel.red)) {
						return 1;
					}
					if (ScanlineImage_To_iPixelSequence_chewchew(self, &pixel.green)) {
						return 1;
					}
					if (ScanlineImage_To_iPixelSequence_chewchew(self, &pixel.blue)) {
						return 1;
					}
					pixel.alpha = 255;
					break;
				case 16:
					if (ScanlineImage_To_iPixelSequence_chewchew16as8(self, &pixel.red)) {
						return 1;
					}
					if (ScanlineImage_To_iPixelSequence_chewchew16as8(self, &pixel.green)) {
						return 1;
					}
					if (ScanlineImage_To_iPixelSequence_chewchew16as8(self, &pixel.blue)) {
						return 1;
					}
					pixel.alpha = 255;
					break;
				default:
					return 1;
			}
			break;
		case 0: //grey
		case 3: //index
		case 4: //grey alph
		case 6: //rgba
		default:
			return 1;
	}

	if (nDestination != NULL) {
		*nDestination = pixel;
	}

	self->pixelPositionX++;
	if (self->pixelPositionX >= self->image.width) {
		self->pixelPositionX = 0;
		return iPixelSequence_ENDOFTHISLINE;
	}

	return 0;
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
	printf("\nmike final error status: 0x%x\n\n", e);

	fclose(f);
	bt = (struct iByteTrain){0};

	struct iPixelSequence pxs = ScanlineImage_as_iPixelSequence(&image);

	struct iPixelSequence_Pixel pxl = {0};
	int csl = 0;
	while (1) {
		e = iPixelSequence_next(&pxs, &pxl);
		printf("rgba: %x %x %x %x\n", pxl.red, pxl.green, pxl.blue, pxl.alpha);
		switch (e) {
			case 0:
				break;
			case iPixelSequence_END:
				printf("peaceful end.\n");
				goto endwhile;
			case iPixelSequence_ENDOFTHISLINE:
				printf("SCANLINE END: %d\n", csl);
				csl++;
				break;
			default:
				printf("error: %d\n", e);
				goto endwhile;
		}

		continue;

		endwhile:
		break;
	}

	return 0;
}
