#ifndef ZOOP_LENGTHDIST_H
#define ZOOP_LENGTHDIST_H

#include <stdint.h>

struct Gunc_BitStream;
struct Gunc_iByteWriter;
struct Gunc_iByteLooker;

int Zoop_getLength(struct Gunc_BitStream *bis, uint16_t *symbol);

int Zoop_getFixedDist(struct Gunc_BitStream *bis, uint16_t *distDest);

int Zoop_nostalgize(struct Gunc_iByteWriter *bw, struct Gunc_iByteLooker *bl, uint16_t length, uint16_t distance);

#endif
