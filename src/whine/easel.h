#ifndef WHINE_Easel_H
#define WHINE_Easel_H

#include "./imHeader.h"

#include "gunc/arr.h"

#include <stdint.h>

#define Whine_Easel_PLTE_ENTRY 3
#define Whine_Easel_PLTE_MAX 256
#define Whine_Easel_PLTE_MIN 1

struct Whine_Easel {
	struct Whine_ImHeader header;

	struct Gunc_ByteArr32 palette;

	//tRNS
	//gAMA
	//cHRM
};

void Whine_Easel_filicide(struct Whine_Easel *self);

int Whine_Easel_indexPalette(const struct Whine_Easel *self, uint8_t index, uint32_t *nDestination);

#endif
