#include "./easel.h"

#include <stdlib.h>


void Whine_Easel_filicide(struct Whine_Easel *self) {

	Gunc_Arr_nfree(&self->palette);

	return;
}

int Whine_Easel_indexPalette(const struct Whine_Easel *self, uint8_t index, uint32_t *nDestination) {
	if (self->palette.data == NULL) {
		return __LINE__;
	}
	if (((uint16_t)index) * Whine_Easel_PLTE_ENTRY >= self->palette.length) {
		return __LINE__;
	}

	if (nDestination == NULL) {
		return 0;
	}

	for (int i = 0; i < Whine_Easel_PLTE_ENTRY; ++i) {
		*nDestination <<= 8;
		*nDestination |= self->palette.data[((uint16_t)index) * Whine_Easel_PLTE_ENTRY + i];
	}

	return 0;
}
