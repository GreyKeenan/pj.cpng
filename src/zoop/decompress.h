#ifndef ZOOP_DECOMPRESS_H
#define ZOOP_DECOMPRESS_H

struct Gunc_iByteStream;
struct Gunc_iByteWriter;
struct Gunc_iByteLooker;

int Zoop_decompress(struct Gunc_iByteStream *bs, struct Gunc_iByteWriter *bw, struct Gunc_iByteLooker *bl);
/*
decompresses a stream of zlib-formatted data from $bs
specifically, expects CM8, or the PNG zlib type
writes decompressed data to $bw
returns 0 on success
*/

#endif
