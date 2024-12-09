#ifndef WHINE_CHUNKS_H
#define WHINE_CHUNKS_H

#include "./reads.h"

#include "gunc/log.h"
#include "gunc/iByteStream.h"
#include "gunc/ascii.h"

#include <stdbool.h>
#include <stddef.h>

struct Gunc_iByteStream;

#define Whine_chunk_NAMELENGTH 4
#define Whine_chunk_CRCLENGTH 4

int Whine_chunk_readName(struct Gunc_iByteStream *bs, char destination[Whine_chunk_NAMELENGTH]);

static inline bool Whine_chunk_isAncillary(char name[Whine_chunk_NAMELENGTH]) {
	return name[0] & Gunc_Ascii_ISCAPITAL;
}
static inline bool Whine_chunk_isPrivate(char name[Whine_chunk_NAMELENGTH]) {
	return name[1] & Gunc_Ascii_ISCAPITAL;
}
static inline bool Whine_chunk_isReserved(char name[Whine_chunk_NAMELENGTH]) {
	return name[2] & Gunc_Ascii_ISCAPITAL;
}
static inline bool Whine_chunk_isCopyable(char name[Whine_chunk_NAMELENGTH]) {
	return name[3] & Gunc_Ascii_ISCAPITAL;
}

static inline int Whine_chunk_eatCRC(struct Gunc_iByteStream *bs) {
	Gunc_TODO("trashing CRC instead of reading it");

	int e = 0;

	for (int i = 0; i < Whine_chunk_CRCLENGTH; ++i) {
		e = Gunc_iByteStream_next(bs, NULL);
		if (e) {
			Gunc_nerr(e, "failed to read");
			return 1;
		}
	}

	return 0;
}

#endif
