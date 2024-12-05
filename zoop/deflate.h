#ifndef ZOOP_deflate_H
#define ZOOP_deflate_H

struct Gunc_BitStream;
struct Gunc_iByteWriter;
struct Gunc_iByteLooker;

int Zoop_deflate(struct Gunc_BitStream *bis, struct Gunc_iByteWriter *bw, struct Gunc_iByteLooker *bl);
/*
decodes DEFLATE stream
*/

#endif
