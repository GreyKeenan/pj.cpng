#ifndef WHINE_unfilter_H
#define WHINE_unfilter_H

struct Whine_Image;
struct Gunc_iByteStream;
struct Gunc_iByteWriter;

int Whine_nofilter(struct Whine_Image *image, struct Gunc_iByteStream *bs);
/*
undoes PNG filetering post-zlib-decompression
mostly assumes a valid Image/ihdr data
gives scanline data to Image
*/

#endif
