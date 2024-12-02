#ifndef WHINE_CHUNKS_H
#define WHINE_CHUNKS_H

#include "gunc/ascii.h"

#include <stdbool.h>

struct Gunc_iByteStream;

#define Whine_chunk_NAMELENGTH 4

int Whine_chunk_readName(struct Gunc_iByteStream *bs, char destination[Whine_chunk_NAMELENGTH]);

static inline bool Whine_chunk_isPrivate(char name[Whine_chunk_NAMELENGTH]) {
	return name[0] & Gunc_Ascii_ISCAPITAL;
}
static inline bool Whine_chunk_isAncillary(char name[Whine_chunk_NAMELENGTH]) {
	return name[1] & Gunc_Ascii_ISCAPITAL;
}
static inline bool Whine_chunk_isReserved(char name[Whine_chunk_NAMELENGTH]) {
	return name[2] & Gunc_Ascii_ISCAPITAL;
}
static inline bool Whine_chunk_isCopyable(char name[Whine_chunk_NAMELENGTH]) {
	return name[3] & Gunc_Ascii_ISCAPITAL;
}

#endif
