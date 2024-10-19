
#include "./stepTree.h"

#include "./iNostalgicWriter.h"

#define MIN 3
int Puff_stepTree_lengthSymbol(uint16_t length, uint16_t *baseValue, uint8_t *numExtraBits) {

	if (length == 285) {
		*numExtraBits = 0;
		*baseValue = 258;
		return 0;
	}
	if (length < 257 || 285 < length) {
		return 1;
	}

	uint16_t l = length - 257;

	if (l < 8) {
		*numExtraBits = 0;
		*baseValue = l + MIN;
		return 0;
	}

	uint8_t df = l / 4;
	uint8_t extraBits = df - 1;
	
	*numExtraBits = extraBits;
	*baseValue = (2 << df) + (l % 4 * (1 << extraBits)) + MIN;

	return 0;
}

int Puff_stepTree_distanceSymbol(uint8_t distance, uint16_t *baseValue, uint8_t *numExtraBits) {
	if (distance > 29) {
		return 1;
	}

	if (distance < 4) {
		*numExtraBits = 0;
		*baseValue = distance + 1;
		return 0;
	}

	uint8_t df = distance / 2;
	uint8_t extraBits = df - 1;

	*numExtraBits = extraBits;
	*baseValue = (1 << df) + ( (distance % 2) * (1 << extraBits) ) + 1;

	return 0;
}

int Puff_stepTree_lz77ify(struct Puff_iNostalgicWriter *nw, uint16_t length, uint16_t distance) {

	if (length < 3 || 258 < length) {
		return 1;
	}
	if (distance < 1 || 32768 < distance) {
		return 2;
	}

	int e = 0;
	uint8_t byte = 0;

	for (int i = 0; i < length; ++i) {
		e = Puff_iNostalgicWriter_nostalgize(nw, &byte, distance);
		if (e) return 3;

		e = Puff_iNostalgicWriter_write(nw, byte);
		if (e) return 4;
	}

	return 0;
}

