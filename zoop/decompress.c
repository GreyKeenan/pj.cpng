#include "./decompress.h"

#include "./alderman.h"
#include "./deflate.h"

#include "gunc/log.h"
#include "gunc/iByteStream.h"
#include "gunc/iByteWriter.h"
#include "gunc/iByteLooker.h"
#include "gunc/bitStream.h"

#include <stdint.h>

static inline int Zoop_header(struct Gunc_iByteStream *bs);
static inline int Zoop_checkAdler(struct Gunc_iByteStream *bs, uint32_t computedAdler);

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

	
	struct Zoop_Alderman john = {0};
	e = Zoop_Alderman_init(&john, bw);
	if (e) {
		Gunc_nerr(e, "failed to elect alderman.");
		return __LINE__;
	}
	struct Gunc_iByteWriter aldermanBw = {0};
	e = Zoop_Alderman_as_iByteWriter(&john, &aldermanBw);
	if (e) {
		Gunc_nerr(e, "failed to init iByteWriter");
		return __LINE__;
	}
	struct Gunc_BitStream bis = { .bys = *bs };
	e = Zoop_deflate(&bis, &aldermanBw, bl);
	if (e) {
		Gunc_nerr(e, "failed to deflate data");
		return __LINE__;
	}

	e = Zoop_checkAdler(bs, Zoop_Alderman_total(&john));
	if (e) {
		Gunc_nerr(e, "couldn't confirm adler");
		return __LINE__;
	}

	return 0;
}

static inline int Zoop_header(struct Gunc_iByteStream *bs) {

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

static inline int Zoop_checkAdler(struct Gunc_iByteStream *bs, uint32_t computedAdler) {
	
	int e = 0;

	uint8_t byte = 0;
	uint32_t adler = 0;

	for (int i = 0; i < 4; ++i) {
		e = Gunc_iByteStream_next(bs, &byte);
		if (e) {
			Gunc_nerr(e, "failed to read byte[%d] of adler", i);
			return 1;
		}
		adler <<= 8;
		adler |= byte;
	}
	Gunc_say("adler: 0x%x", adler);


	if (adler != computedAdler) {
		Gunc_err("Adler32 check failed! read: 0x%x computed: 0x%x", adler, computedAdler);
		return 2;
	}
	Gunc_say("adler check passed!");

	return 0;
}
