#ifndef ZOOP_walkLitTree_H
#define ZOOP_walkLitTree_H

struct Shrub_Tree;
struct Gunc_BitStream;
struct Gunc_iByteWriter;
struct Gunc_iByteLooker;

int Zoop_walkLitTree(const struct Shrub_Tree *tree, struct Gunc_BitStream *bis, struct Gunc_iByteWriter *bw, struct Gunc_iByteLooker *bl);

#endif
