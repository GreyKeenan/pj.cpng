#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "utils/iByteTrain_forw.h"
#include "utils/iByteTrain_impl.h"
#include "utils/iByteTrain.h"

#include "mike/mike.h"
#include "mike/scanlineImage_impl.h"
#include "mike/scanlineImage_forw.h"

/*
#include "iPixelSequence/iPixelSequence.h"
#include "iPixelSequence/iPixelSequence_impl.h"
#include "iPixelSequence/iPixelSequence_forw.h"
#include "iPixelSequence/pixel_forw.h"
#include "iPixelSequence/pixel_impl.h"
*/

#include "sdaubler/display.h"
#include "sdaubler/iImageTrain_impl.h"
#include "sdaubler/iImageTrain_forw.h"

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


#include "temp_scanlineImageTrain.h" //TODO TEMP

int main(int argc, const char **argv) {

	// file
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	struct iByteTrain bt = {0};
	//FILE *f = FILE_as_iByteTrain("pngs/PNG_transparency_demonstration.png", &bt);
	FILE *f = FILE_as_iByteTrain("pngs/uncompressed.png", &bt);
	if (f == NULL) {
		printf("cant open file\n");
		return 1;
	}
	
	// decode
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	struct Mike_ScanlineImage image = {0};

	int e = Mike_decode(&bt, &image);
	printf("\nmike final error status: 0x%x\n\n", e);
	if (e) return e;

	fclose(f);
	bt = (struct iByteTrain){0};

	// display
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	struct Imbridge bridge = { .image = &image };

	struct Sdaubler_iImageTrain imt = {0};
	imt = Imbridge_as_iImageTrain(&bridge);

	e = Sdaubler_display(&imt);
	printf("\nsdaubler error status: 0x%x\n\n", e);
	if (e) return e;


	//end
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//TODO free image data
	return 0;
}
