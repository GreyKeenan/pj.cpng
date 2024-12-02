#include "./decompress.h"

#include "gunc/log.h"
#include "gunc/iByteStream.h"
#include "gunc/iByteWriter.h"
#include "gunc/iByteLooker.h"

#include <stdint.h>

int Zoop_header(struct Gunc_iByteStream *bs);

#define CM_PNG 8
#define ZLIB_MODCHECK 31
#define ZLIB_HASDICT 0x01


int Zoop_decompress(struct Gunc_iByteStream *bs, struct Gunc_iByteWriter *bw, struct Gunc_iByteLooker *bl) {

	int e = 0;

	e = Zoop_header(bs);
	if (e) {
		Gunc_nerr(e, "ZLIB header read failed");
		return __LINE__;
	}
	Gunc_say("ZLIB header confirmed");


	Gunc_TODO("this function");
	return -999;
}

int Zoop_header(struct Gunc_iByteStream *bs) {

	int e = 0;
	uint8_t b = 0;
	uint16_t check = 0;

	e = Gunc_iByteStream_next(bs, &b);
	if (e) {
		Gunc_nerr(e, "failed read CMF");
		return __LINE__;
	}
	check |= b;
	check <<= 8;
	if ((b & 0x0f) != CM_PNG) {
		Gunc_err("CM should be 8 for PNGs. Got: %d", b);
		return __LINE__;
	}
	if ((b >> 4) > 7) {
		Gunc_err("window size indicator cannot exceed 7. Got: %d", b);
		return __LINE__;
	}

	e = Gunc_iByteStream_next(bs, &b);
	if (e) {
		Gunc_nerr(e, "failed to read FLG");
		return __LINE__;
	}
	check |= b;
	if (check % ZLIB_MODCHECK) {
		Gunc_err("zlib header check failed: %d", check);
		return __LINE__;
	}
	b >>= 5;
	if (b & ZLIB_HASDICT) {
		Gunc_err("DICT bit set. PNGs shouldnt have dict.");
		return __LINE__;
	}

	// 2b compression level indicator, irrelevant

	return 0;
}
