#ifndef WHINE_IHDR_H
#define WHINE_IHDR_H

#include "./image.h"
#include "./reads.h"
#include "./chunk.h"

#include "gunc/log.h"
#include "gunc/iByteStream.h"

#include <stdint.h>
#include <string.h>

static inline int Whine_ihdr(struct Whine_Image *destination, struct Gunc_iByteStream *bs);
static inline int Whine_validateBDCT(struct Whine_Image *ihdr);

#define Whine_ihdr_LENGTH 13

static inline int Whine_ihdr(struct Whine_Image *destination, struct Gunc_iByteStream *bs) {
	
	int e = 0;
	uint32_t length = 0;
	char name[Whine_chunk_NAMELENGTH + 1] = {0};

	e = Whine_read_int32(bs, &length);
	if (e) {
		Gunc_nerr(e, "failed to read length");
		return __LINE__;
	}
	if (length != Whine_ihdr_LENGTH) {
		Gunc_err("Invalid IHDR length (%d). Should be 13.", length);
		return __LINE__;
	}

	e = Whine_chunk_readName(bs, name);
	if (e) {
		Gunc_nerr(e, "Unable to read chunk name.");
		return __LINE__;
	}
	if (strcmp(name, "IHDR")) {
		Gunc_err("invalid IHDR name: \"%s\"", name);
		return __LINE__;
	}

	e = Whine_read_int32(bs, &destination->w);
	if (e) {
		Gunc_nerr(e, "failed to read width");
		return __LINE__;
	}
	if (destination->w == 0) {
		Gunc_err("invalid width (%d)", destination->w);
		return __LINE__;
	}

	e = Whine_read_int32(bs, &destination->h);
	if (e) {
		Gunc_nerr(e, "failed to read height");
		return __LINE__;
	}
	if (destination->h == 0) {
		Gunc_err("invalid height (%d)", destination->h);
		return __LINE__;
	}

	e = Gunc_iByteStream_next(bs, &destination->bitDepth);
	if (e) {
		Gunc_nerr(e, "failed to read bitDepth");
		return __LINE__;
	}
	e = Gunc_iByteStream_next(bs, &destination->colorType);
	if (e) {
		Gunc_nerr(e, "failed to read colorType");
		return __LINE__;
	}
	e = Whine_validateBDCT(destination);
	if (e) {
		Gunc_nerr(e, "invalid bitDepth (%d) or colorType (%d)", destination->bitDepth, destination->colorType);
		return __LINE__;
	}

	e = Gunc_iByteStream_next(bs, &destination->compressionMethod);
	if (e) {
		Gunc_nerr(e, "failed to read compressionMethod");
		return __LINE__;
	}
	if (destination->compressionMethod != 0) {
		Gunc_err("invalid compressionMethod: %d", destination->compressionMethod);
		return __LINE__;
	}

	e = Gunc_iByteStream_next(bs, &destination->filterMethod);
	if (e) {
		Gunc_nerr(e, "failed to read filterMethod");
		return __LINE__;
	}
	if (destination->filterMethod != 0) {
		Gunc_err("invalid filterMethod: %d", destination->filterMethod);
		return __LINE__;
	}

	e = Gunc_iByteStream_next(bs, &destination->interlaceMethod);
	if (e) {
		Gunc_nerr(e, "failed to read interlaceMethod");
		return __LINE__;
	}
	if (destination->interlaceMethod > 1) {
		Gunc_err("invalid interlaceMethod: %d", destination->interlaceMethod);
		return __LINE__;
	}

	e = Whine_chunk_eatCRC(bs);
	if (e) {
		Gunc_nerr(e, "eatCRC failed");
		return __LINE__;
	}

	return 0;
}

static inline int Whine_validateBDCT(struct Whine_Image *ihdr) {

	#define Whine_BDLENGTH 5
	uint8_t validBitDepths[Whine_BDLENGTH] = {1, 2, 4, 8, 16};

	switch (ihdr->bitDepth) {
		case 1:
		case 2:
		case 4:
		case 8:
		case 16:
			break;
		default:
			Gunc_err("invalid bitDepth: %d", ihdr->bitDepth);
			return 1;
	}
	switch (ihdr->colorType) {
		case 0:
			break;
		case 3:
			validBitDepths[4] = 0;
			break;
		case 2:
		case 4:
		case 6:
			validBitDepths[0] = 0;
			validBitDepths[1] = 0;
			validBitDepths[2] = 0;
			break;
		default:
			Gunc_err("invalid colorType: %d", ihdr->colorType);
			return 2;
	}

	for (int i = 0; i < Whine_BDLENGTH; ++i) {
		if (ihdr->bitDepth == validBitDepths[i]) {
			return 0;
		}
	}

	Gunc_err("invalid bitDepth colorType combo: (%d)/(%d)", ihdr->bitDepth, ihdr->colorType);
	return 3;
}

#endif
