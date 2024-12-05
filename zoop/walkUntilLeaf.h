#ifndef ZOOP_walkUntilLeaf_H
#define ZOOP_walkUntilLeaf_H

#include <stdint.h>

struct Shrub_Tree;
struct Gunc_BitStream;

int Zoop_walkUntilLeaf(const struct Shrub_Tree *tree, struct Gunc_BitStream *bis, uint16_t *nDestination);

#endif
