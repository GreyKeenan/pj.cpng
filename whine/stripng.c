#include "./stripng.h"

#include "./image.h"
#include "./chunk.h"
#include "./ihdr.h"

#include "gunc/log.h"
#include "gunc/iByteStream.h"
#include "gunc/iByteWriter.h"

#include <stdint.h>
#include <string.h>

static inline int Whine_header(struct Gunc_iByteStream *bs);
static inline int Whine_chunkstream(struct Gunc_iByteStream *bs, struct Gunc_iByteWriter *bw);
static inline int Whine_waste(struct Gunc_iByteStream *bs, uint32_t count);

#define HEADER_LENGTH 8
const uint8_t Whine_HEADER[HEADER_LENGTH] = {0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A};


int Whine_stripng(struct Whine_Image *destination, struct Gunc_iByteStream *bs, struct Gunc_iByteWriter *bw) {

	int e = 0;

	e = Whine_header(bs);
	if (e) {
		return 1;
	}
	Gunc_say("PNG header matched.");

	e = Whine_ihdr(destination, bs);
	if (e) {
		Gunc_nerr(e, "failed to read IHDR");
		return 2;
	}
	Gunc_say("ihdr confirmed.");

	e = Whine_chunkstream(bs, bw);
	if (e) {
		Gunc_nerr(e, "failed to process chunk sequence.");
		return 3;
	}

	return 0;
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

static inline int Whine_chunkstream(struct Gunc_iByteStream *bs, struct Gunc_iByteWriter *bw) {

	int e = 0;
	uint32_t length = 0;
	char name[Whine_chunk_NAMELENGTH + 1] = {0};
	uint8_t b = 0;

	while (1) {

		e = Whine_read_int32(bs, &length);
		if (e) {
			Gunc_nerr(e, "failed to read length");
			return __LINE__;
		}
		Gunc_say("chunk length: %d (0x%x)", length, length);

		e = Whine_chunk_readName(bs, name);
		if (e) {
			Gunc_nerr(e, "failed to read name");
			return __LINE__;
		}
		Gunc_say("chunk name: %s", name);

		if (Whine_chunk_isAncillary(name)) {
			Whine_waste(bs, length + Whine_chunk_CRCLENGTH);
			continue;
		}

		if (!strcmp(name, "IEND")) {
			break;
		}

		/* TODO
		if (!strcmp(name, "PLTE")) {
			// ...
			goto finishChunk;
		}

		if (!strcmp(name, "IDAT")) {
			// ...
			goto finishChunk;
		}
		*/

		Gunc_err("Unrecognized critical chunk: %s", name);
		return __LINE__;


		finishChunk:
		e = Whine_chunk_eatCRC(bs);
		if (e) {
			Gunc_nerr(e, "failed to eat crc");
			return __LINE__;
		}
	}
	e = Whine_chunk_eatCRC(bs);
	if (e) {
		Gunc_nerr(e, "failed to eat crc for IEND");
		return __LINE__;
	}

	Gunc_TODO("validate idat/plte exist");
	return __LINE__;
}

static inline int Whine_waste(struct Gunc_iByteStream *bs, uint32_t count) {
	int e = 0;
	for (uint32_t i = 0; i < count; ++i) {
		e = Gunc_iByteStream_next(bs, NULL);
		if (e) {
			return e;
		}
	}
	return 0;
}