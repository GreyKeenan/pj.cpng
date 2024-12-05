#include "./lengthDist.h"

#include "gunc/log.h"
#include "gunc/bitStream.h"
#include "gunc/iByteWriter.h"
#include "gunc/iByteLooker.h"

#include <stdbool.h>

#define FIXED_DISTCODELENGTH 5

static inline int Zoop_lengthSymbol(uint16_t length, uint16_t *baseValue, uint16_t *extraBits);
static inline int Zoop_distanceSymbol(uint16_t distance, uint16_t *baseValue, uint16_t *extraBits);
static inline int Zoop_extraBits(struct Gunc_BitStream *bis, uint8_t extraBits, uint16_t *destination);


int Zoop_getLength(struct Gunc_BitStream *bis, uint16_t *symbol) {

	int e = 0;
	bool bit = 0;

	uint16_t baseValue = 0;
	uint16_t extraBits = 0;

	e = Zoop_lengthSymbol(*symbol, &baseValue, &extraBits);
	if (e) {
		Gunc_nerr(e, "unable to interperet length symbol: %d", *symbol);
		return __LINE__;
	}

	uint16_t additional = 0;

	e = Zoop_extraBits(bis, extraBits, &additional);
	if (e) {
		Gunc_nerr(e, "unable to grab extra bits (%d)", extraBits);
		return __LINE__;
	}

	//Gunc_say("length symbol: %d base: %d extraBits: %d additional: %d", *symbol, baseValue, extraBits, additional);

	*symbol = baseValue + additional;

	return 0;
}

int Zoop_getFixedDist(struct Gunc_BitStream *bis, uint16_t *distDest) {

	int e = 0;
	bool bit = 0;

	uint16_t symbol = 0;

	for (int i = 0; i < FIXED_DISTCODELENGTH; ++i) {
		e = Gunc_BitStream_bit(bis, &bit);
		if (e) {
			Gunc_nerr(e, "bit read fail");
			return __LINE__;
		}
		symbol |= bit << (FIXED_DISTCODELENGTH - i - 1);
		//Gunc_say("bit: %d (0x%x)", bit, symbol);
	}

	uint16_t baseValue = 0;
	uint16_t extraBits = 0;

	e = Zoop_distanceSymbol(symbol, &baseValue, &extraBits);
	if (e) {
		Gunc_nerr(e, "failed to decipher distance symbol: %d", symbol);
		return __LINE__;
	}

	uint16_t additional = 0;

	e = Zoop_extraBits(bis, extraBits, &additional);
	if (e) {
		Gunc_nerr(e, "unable to grab extra bits (%d)", extraBits);
		return __LINE__;
	}

	Gunc_say("distance symbol: %d base: %d extraBits: %d additional: %d", symbol, baseValue, extraBits, additional);

	*distDest = baseValue + additional;

	return 0;
}

int Zoop_nostalgize(struct Gunc_iByteWriter *bw, struct Gunc_iByteLooker *bl, uint16_t length, uint16_t distance) {

	// ? validate length & distance
	//Gunc_say("nostalgizing: %d / %d", length, distance);

	int e = 0;
	uint8_t byte = 0;

	for (int i = 0; i < length; ++i) {

		e = Gunc_iByteLooker_look(bl, &byte, (int32_t)-1 * distance);
		if (e) {
			Gunc_nerr(e, "failed to look (%d)", i);
			return 1;
		}

		Gunc_say("writing 0x%x from %d back", byte, (int32_t)-1 * distance);

		e = Gunc_iByteWriter_give(bw, byte);
		if (e) {
			Gunc_nerr(e, "failed to write byte 0x%x. (%d)", byte, i);
			return 2;
		}

	}

	return 0;
}


static inline int Zoop_extraBits(struct Gunc_BitStream *bis, uint8_t extraBits, uint16_t *destination) {

	int e = 0;
	bool bit = 0;

	uint16_t extra = 0;

	for (int i = 0; i < extraBits; ++i) {

		e = Gunc_BitStream_bit(bis, &bit);
		if (e) {
			Gunc_nerr(e, "failed bit read");
			return __LINE__;
		}

		extra |= bit << i;
	}

	*destination = extra;
	return 0;
}

#define LMIN 3
static inline int Zoop_lengthSymbol(uint16_t length, uint16_t *baseValue, uint16_t *extraBits) {
	if (length <= 256 || length > 285) {
		Gunc_err("invalid length");
		return 1;
	}

	if (length == 285) {
		*extraBits = 0;
		*baseValue = 258;
		return 0;
	}

	length -= 257;

	if (length < 8) {
		*extraBits = 0;
		*baseValue = length + LMIN;
		return 0;
	}

	uint8_t df = length / 4;
        uint8_t exBits = df - 1;

        *extraBits = exBits;
        *baseValue = (2 << df) + (length % 4 * (1 << exBits)) + LMIN;

	return 0;
}

#define DMIN 1
static inline int Zoop_distanceSymbol(uint16_t distance, uint16_t *baseValue, uint16_t *extraBits) {
	if (distance > 29) {
		return 1;
	}

	if (distance < 4) {
		*extraBits = 0;
		*baseValue = distance + DMIN;
		return 0;
	}

	uint8_t df = distance / 2;
	uint8_t exBits = df - 1;

	*extraBits = exBits;
	*baseValue = (1 << df) + ( (distance % 2) * (1 << exBits) ) + DMIN;

	return 0;
}
