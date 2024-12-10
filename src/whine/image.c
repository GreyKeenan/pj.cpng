#include "./image.h"

#include <stdlib.h>


void Whine_Image_filicide(struct Whine_Image *self) {
	if (self == NULL) {
		return;
	}

	if (self->hnImageData != NULL) {
		free(self->hnImageData);
		self->hnImageData = NULL;
		self->imageDataStatus = Whine_Image_ABSENT;
	}
	if (self->hnPalette != NULL) {
		free(self->hnPalette);
		self->hnPalette = NULL;
		self->paletteLength = 0;
	}

	return;
}
