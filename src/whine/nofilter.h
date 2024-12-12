#ifndef WHINE_unfilter_H
#define WHINE_unfilter_H

struct Whine_Easel;
struct Gunc_iByteStream;
struct Gunc_iByteWriter;

int Whine_nofilter(struct Gunc_iByteStream *bs, struct Gunc_iByteWriter *bw, const struct Whine_Easel *easel);
/*
undoes PNG filetering post-zlib-decompression
*/

#endif
