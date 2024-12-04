#ifndef SHRUB_Tree_CORE_H
#define SHRUB_Tree_CORE_H

#include "./tree.h"

#include <stdint.h>
#include <stdbool.h>

static inline int Shrub_Tree_get(const struct Shrub_Tree *self, uint16_t index, uint32_t *destination);
static inline int Shrub_Tree_set(struct Shrub_Tree *self, uint16_t index, uint32_t node);

static inline uint16_t Shrub_Tree_shiftOut(uint32_t node, uint8_t bitsPerChild, bool isRight);
static inline void Shrub_Tree_shiftIn(uint32_t *node, uint8_t bitsPerChild, bool isRight, uint16_t child, bool type);


static inline int Shrub_Tree_get(const struct Shrub_Tree *self, uint16_t index, uint32_t *destination) {
	if (index >= self->currentNodes) { return 1; }

	*destination = 0;
	uint8_t *at = self->data + (self->bytesPerNode * (uint32_t)index);
	for (int i = 0; i < self->bytesPerNode; ++i) {
		*destination |= at[i] << (i * 8);
	}

	return 0;
}
static inline int Shrub_Tree_set(struct Shrub_Tree *self, uint16_t index, uint32_t node) {
	if (index >= self->currentNodes) { return 1; }

	uint8_t *at = self->data + (self->bytesPerNode * (uint32_t)index);
	for (int i = 0; i < self->bytesPerNode; ++i) {
		at[i] = node & 0xff;
		node >>= 8;
	}

	return 0;
}

static inline uint16_t Shrub_Tree_shiftOut(uint32_t node, uint8_t bitsPerChild, bool isRight) {
	//yes, the right child is stored on the left side.
	return (node >> (bitsPerChild * isRight)) & ((1 << bitsPerChild) - 1);
}
static inline void Shrub_Tree_shiftIn(uint32_t *node, uint8_t bitsPerChild, bool isRight, uint16_t child, bool type) {

	child <<= 1;
	child |= type;

	uint32_t mask = 1 << bitsPerChild;
	mask--;
	mask <<= bitsPerChild * !isRight;
	*node &= mask; // set everything except the opposite child to 0s

	*node |= ((uint32_t)child) << (bitsPerChild * isRight);

	//*node = (*node & ((1 << bitsPerChild + 1) - 1 << (bitsPerChild + 1) * !isRight)) | (((uint32_t)child << 1 | type) << (bitsPerChild + 1) * isRight);
		//unconfirmed, copied from prev implementation. More difficult to read imo.
}

#endif
