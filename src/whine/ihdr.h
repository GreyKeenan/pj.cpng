#ifndef WHINE_IHDR_H
#define WHINE_IHDR_H

#include "./imHeader.h"
#include "./reads.h"
#include "./chunk.h"

#include "gunc/log.h"
#include "gunc/iByteStream.h"

#include <stdint.h>
#include <string.h>

#define Whine_ihdr_LENGTH 13

static inline int Whine_ihdr(struct Whine_ImHeader *header, struct Gunc_iByteStream *bs) {
	
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

	uint32_t n = 0;

	e = Whine_read_int32(bs, &n);
	if (e) {
		Gunc_nerr(e, "failed to read width");
		return __LINE__;
	}
	header->width = n;
	e = Whine_read_int32(bs, &n);
	if (e) {
		Gunc_nerr(e, "failed to read height");
		return __LINE__;
	}
	header->height = n;
	e = Gunc_iByteStream_next(bs, &header->bitDepth);
	if (e) {
		Gunc_nerr(e, "failed to read bitDepth");
		return __LINE__;
	}
	e = Gunc_iByteStream_next(bs, &header->colorType);
	if (e) {
		Gunc_nerr(e, "failed to read colorType");
		return __LINE__;
	}
	e = Gunc_iByteStream_next(bs, &header->compressionMethod);
	if (e) {
		Gunc_nerr(e, "failed to read compressionMethod");
		return __LINE__;
	}
	e = Gunc_iByteStream_next(bs, &header->filterMethod);
	if (e) {
		Gunc_nerr(e, "failed to read filterMethod");
		return __LINE__;
	}
	e = Gunc_iByteStream_next(bs, &header->interlaceMethod);
	if (e) {
		Gunc_nerr(e, "failed to read interlaceMethod");
		return __LINE__;
	}

	e = Whine_ImHeader_validate(header);
	if (e) {
		Gunc_nerr(e, "invalid IHDR data");
		return __LINE__;
	}

	e = Whine_chunk_eatCRC(bs);
	if (e) {
		Gunc_nerr(e, "eatCRC failed");
		return __LINE__;
	}

	return 0;
}

#endif
