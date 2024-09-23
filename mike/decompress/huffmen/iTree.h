#ifndef MIKE_DECOMPRESS_HUFFMEN_ITREE_H
#define MIKE_DECOMPRESS_HUFFMEN_ITREE_H

#include "./iTree_impl.h"

static inline int Mike_Decompress_Huffmen_iTree_addCode(struct Mike_Decompress_Huffmen_iTree *self, uint16_t code, uint8_t codeLength, uint16_t value) {
	return self->addCode(self->vself, code, codeLength, value);
}

static inline int Mike_Decompress_Huffman_iTree_walk(struct Mike_Decompress_Huffmen_iTree *self, _Bool side, uint16_t *nValue) {
	return self->walk(self->vself, side, nValue);
}

#endif
