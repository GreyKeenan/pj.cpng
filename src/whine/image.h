#ifndef WHINE_IMAGE_H
#define WHINE_IMAGE_H

#include "./imHeader.h"

#include <stdint.h>

enum Whine_Image_imageDataStatus {
	Whine_Image_ABSENT = 0
	, Whine_Image_COMPRESSED
	, Whine_Image_FILTERED
	, Whine_Image_SCANLINED
};

struct Whine_Image {
	struct Whine_ImHeader header;

	uint8_t imageDataStatus;
	uint8_t *hnImageData;

	uint8_t *hnPalette;
	int32_t paletteLength;

	//transparency data
	//gamma
	//chrm
};

#define Whine_Image_PLTE_ENTRY 3
#define Whine_Image_PLTE_MIN 1
#define Whine_Image_PLTE_MAX 256

void Whine_Image_filicide(struct Whine_Image *self);
/*
frees the following if they are not null
	$hnImageData
	$hnPalette
sets them to NULL & resets their associated tracking values
DOES NOT reset $header
*/

static inline void Whine_Image_setData(struct Whine_Image *self, uint8_t *as, uint8_t status) {
	self->hnImageData = as;
	self->imageDataStatus = status;
} // encourages setting status when setting data

/*
static inline int Whine_Image_indexPalette(const struct Whine_Image *self, int32_t index, uint32_t *pixel) {

	
	
	return 1;
}
*/

#endif
