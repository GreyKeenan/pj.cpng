#ifndef Shrub_DynaTree_H
#define Shrub_DynaTree_H

struct Shrub_LitTree;

#include <stdint.h>

int Shrub_DynaTree_init(struct Shrub_LitTree *self, uint8_t *sizes, uint16_t countGiven);

#endif
