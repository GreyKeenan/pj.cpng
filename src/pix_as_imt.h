#ifndef MAIN_pix_as_imt_H
#define MAIN_pix_as_imt_H

#include "whine/pixie.h"

#include "sdaubler/iImageTrain_impl.h"

int _Pixie_gauge(struct Whine_Pixie *self, uint32_t *nWidth, uint32_t *nHeight);

static inline int _Pixie_as_iImageTrain(struct Whine_Pixie *self, struct Sdaubler_iImageTrain *imt) {
	if (self == NULL || imt == NULL) {
		return 1;
	}

	*imt = (struct Sdaubler_iImageTrain) {
		.vself = self,
		.gauge = (int(*)(const void*, uint32_t*, uint32_t*))&_Pixie_gauge,
		.choochoo = (int(*)(void*, uint32_t*))&Whine_Pixie_nextPixel
	};

	return 0;
}

#endif
