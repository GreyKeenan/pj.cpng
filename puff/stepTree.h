#ifndef PUFF_STEPTREE_H
#define PUFF_STEPTREE_H

#include "./state_impl.h"

#include <stdint.h>

struct Puff_iNostalgicWriter;

int Puff_stepTree_lengthSymbol(uint16_t length, uint16_t *baseValue, uint8_t *numExtraBits);
int Puff_stepTree_distanceSymbol(uint8_t distance, uint16_t *baseValue, uint8_t *numExtraBits);
int Puff_stepTree_lz77ify(struct Puff_iNostalgicWriter *nw, uint16_t length, uint16_t distance);
//static inline _Bool Puff_stepTree_collectBits(struct Puff_State_BitCollector *bc, _Bool bit);

static inline _Bool Puff_stepTree_collectBits(struct Puff_State_BitCollector *bc, _Bool bit) {

	if (bc->collectFor & Puff_State_COLLECTFOR_LSBIT) {
		bc->bits |= bit << (bc->collected);
		bc->collected++;
		return bc->collected < bc->max;
	}

	bc->bits |= bit << (bc->max - 1 - bc->collected);
	bc->collected++;
	return bc->collected < bc->max;
}

#endif
