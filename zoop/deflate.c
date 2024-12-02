#include "./deflate.h"

#include "gunc/log.h"
#include "gunc/bitStream.h"
#include "gunc/iByteWriter.h"
#include "gunc/iByteLooker.h"

#include <stdint.h>
#include <stddef.h>

static inline int Zoop_readInt16(struct Gunc_BitStream *bis, uint16_t *nDestination);
static inline int Zoop_uncompressed(struct Gunc_BitStream *bis, struct Gunc_iByteWriter *bw);


int Zoop_deflate(struct Gunc_BitStream *bis, struct Gunc_iByteWriter *bw, struct Gunc_iByteLooker *bl) {

	int e = 0;
	bool bit = 0;

	bool isLastBlock = false;
	uint8_t compressionType = 0;

	do {
		e = Gunc_BitStream_bit(bis, &isLastBlock);
		if (e) {
			Gunc_nerr(e, "failed to fetch bit");
			return __LINE__;
		}
		Gunc_say("is last block: %d", isLastBlock);

		e = Gunc_BitStream_bit(bis, &bit);
		if (e) {
			Gunc_nerr(e, "failed to fetch bit ct0");
			return __LINE__;
		}
		compressionType = bit;
		e = Gunc_BitStream_bit(bis, &bit);
		if (e) {
			Gunc_nerr(e, "failed to fetch bit ct1");
			return __LINE__;
		}
		compressionType |= bit << 1;

		switch (compressionType) {
			case 0:
				e = Zoop_uncompressed(bis, bw);
				if (e) {
					Gunc_nerr(e, "failed to process uncompressed data");
					return __LINE__;
				}
				break;
			case 1:
				Gunc_TODO("fixed compression type");
				break;
			case 2:
				Gunc_TODO("dynamic compression type");
				break;
			case 3:
				Gunc_err("reserved compression type (%d).", compressionType);
				return __LINE__;
			default:
				Gunc_err("unrecognized compressionType. How did this even happen? (%d)", compressionType);
				return __LINE__;
		}
	} while (!isLastBlock);

	return 0;
}

static inline int Zoop_uncompressed(struct Gunc_BitStream *bis, struct Gunc_iByteWriter *bw) {

	int e = 0;
	uint8_t byte = 0;

	uint16_t length = 0;
	uint16_t nLength = 0;

	e = Zoop_readInt16(bis, &length);
	if (e) {
		Gunc_nerr(e, "failed to read length");
		return 1;
	}
	Gunc_say("length: %d", length);
	e = Zoop_readInt16(bis, &nLength);
	if (e) {
		Gunc_nerr(e, "failed to read nLength");
		return 2;
	}
	Gunc_say("nLength: %d", nLength);

	if (length != (~nLength & 0xffff)) {
		Gunc_err("length-inverted-length check failed: 0x%x / 0x%x (~0x%x)", length, nLength, (~nLength & 0xffff));
		return 3;
	}

	
	for (int32_t i = 0; i < length; ++i) {
		e = Gunc_BitStream_nextByte(bis, &byte);
		if (e) {
			Gunc_nerr(e, "failed to read byte #%d", i);
			return 4;
		}
		e = Gunc_iByteWriter_give(bw, byte);
		if (e) {
			Gunc_nerr(e, "failed to write byte #%d: 0x%x", i, byte);
			return 5;
		}
	}

	return 0;
}

static inline int Zoop_readInt16(struct Gunc_BitStream *bis, uint16_t *nDestination) {
	//LSB first as per DEFLATE
	int e = 0;
	uint8_t byte = 0;
	uint16_t n = 0;

	e = Gunc_BitStream_nextByte(bis, &byte);
	if (e) {
		Gunc_nerr(e, "failed to read LSB of int16");
		return 1;
	}
	n |= byte;
	e = Gunc_BitStream_nextByte(bis, &byte);
	if (e) {
		Gunc_nerr(e, "failed to read MSB of int16");
		return 2;
	}
	n |= byte << 8;
	
	if (nDestination != NULL) {
		*nDestination = n;
	}
	return 0;
}
