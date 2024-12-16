#ifndef WHINE_thicken_H
#define WHINE_thicken_H

struct Whine_Easel;
struct Whine_Canvas;
struct Gunc_iByteStream;

int Whine_thicken(const struct Whine_Easel *easel, struct Whine_Canvas *canvas, struct Gunc_iByteStream *bs);
/*
removes PNG filters
also undoes interlacing
*/

#endif
