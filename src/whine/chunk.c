#include "./chunk.h"

#include "gunc/log.h"
#include "gunc/iByteStream.h"

int Whine_chunk_readName(struct Gunc_iByteStream *bs, char destination[Whine_chunk_NAMELENGTH]) {

	int e = 0;
	uint8_t b = 0;

	for (int i = 0; i < Whine_chunk_NAMELENGTH; ++i) {
		e = Gunc_iByteStream_next(bs, &b);
		if (e) {
			Gunc_nerr(e, "unable to read byte");
			return 1;
		}

		if (!Gunc_Ascii_isLetter(b)) {
			Gunc_err("not a letter");
			return 2;
		}

		destination[i] = b;
	}

	return 0;
}
