#include "./stripng.h"

#include "./image.h"
#include "./chunk.h"
#include "./ihdr.h"

#include "gunc/log.h"
#include "gunc/iByteStream.h"
#include "gunc/iByteWriter.h"

#include <stdint.h>

static inline int Whine_header(struct Gunc_iByteStream *bs);

#define HEADER_LENGTH 8
const uint8_t Whine_HEADER[HEADER_LENGTH] = {0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A};


int Whine_stripng(struct Whine_Image *destination, struct Gunc_iByteStream *bs, struct Gunc_iByteWriter *bw) {

	int e = 0;

	e = Whine_header(bs);
	if (e) {
		return __LINE__;
	}
	Gunc_say("PNG header matched.");

	e = Whine_ihdr(destination, bs);
	if (e) {
		Gunc_nerr(e, "failed to read IHDR");
		return __LINE__;
	}
	Gunc_say("ihdr read.");

	return __LINE__;
}

static inline int Whine_header(struct Gunc_iByteStream *bs) {

	int e = 0;
	uint8_t b = 0;

	for (int i = 0; i < HEADER_LENGTH; ++i) {
		e = Gunc_iByteStream_next(bs, &b);
		if (e) {
			Gunc_nerr(e, "failed to read byte");
			return 2;
		}
		if (b != Whine_HEADER[i]) {
			Gunc_err("failed PNG header check #%d. (0x%x) should be (0x%x)", i, b, Whine_HEADER[i]);
			return 1;
		}
	}

	return 0;
}
