#ifndef ZOOP_LENGTHDIST_H
#define ZOOP_LENGTHDIST_H

#include <stdint.h>

#define Zoop_HUFFMAN_END 256

struct Shrub_Tree;

struct Gunc_BitStream;
struct Gunc_iByteWriter;
struct Gunc_iByteLooker;

int Zoop_getLength(struct Gunc_BitStream *bis, uint16_t *symbol);

int Zoop_getFixedDist(struct Gunc_BitStream *bis, uint16_t *distDest);
int Zoop_getDynaDist(struct Shrub_Tree *tree, struct Gunc_BitStream *bis, uint16_t *destination);

int Zoop_nostalgize(struct Gunc_iByteWriter *bw, struct Gunc_iByteLooker *bl, uint16_t length, uint16_t distance);

#endif
