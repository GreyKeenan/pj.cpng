#ifndef WHINE_STRIPNG_H
#define WHINE_STRIPNG_H

struct Whine_Easel;
struct Gunc_iByteStream;
struct Gunc_iByteWriter;

int Whine_stripng(struct Gunc_iByteStream *bs, struct Gunc_iByteWriter *bw, struct Whine_Easel *easel);
/*
expects $bs to be a png
reads png measurements to $destination
gives raw zlib-compression-bytes to $bw
returns 0 on success
*/

#endif
