#include "./pix_as_imt.h"


int _Pixie_gauge(struct Whine_Pixie *self, uint32_t *nWidth, uint32_t *nHeight) {
	if (nWidth != NULL) {
		*nWidth = self->easel.header.width;
	}
	if (nHeight != NULL) {
		*nHeight = self->easel.header.height;
	}
	return 0;
}
