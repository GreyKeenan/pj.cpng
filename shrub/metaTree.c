#include "./metaTree.h"

#include "gunc/log.h"
#include "gunc/bitStream.h"

#include <stdbool.h>

#define BITINT 3
#define MAXSIZE (1 << BITINT)
#define GIVEN_MIN 4
#define ORDERLEN Shrub_MetaTree_MAXLEAVES
//const uint8_t Shrub_MetaTree_ORDER[ORDERLEN] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
	// codeSizes are given in a specific order
	// "lexicographic order" is unchanged for purposes of generating codes (ascending)
	// So, I use UNORDER to access them as organized at the approp time
const uint8_t Shrub_MetaTree_UNORDER[ORDERLEN] = {3, 17, 15, 13, 11, 9, 7, 5, 4, 6, 8, 10, 12, 14, 16, 18, 0, 1, 2};
	// UNORDER[n] gives index of [n] in ORDER


int Shrub_MetaTree_init(struct Shrub_MetaTree *self, struct Gunc_BitStream *bis, uint8_t countGiven) {

	if (self == NULL || bis == NULL) {
		Gunc_err("null");
		return __LINE__;
	}
	if (countGiven > ORDERLEN || countGiven < GIVEN_MIN) {
		Gunc_err("invalid countGiven: %d", countGiven);
		return __LINE__;
	}

	int e = 0;

	uint8_t sizes[ORDERLEN] = {0};
	bool bit = 0;
	for (int i = 0; i < countGiven; ++i) {
		for (int j = 0; j < BITINT; ++j) {
			e = Gunc_BitStream_bit(bis, &bit);
			if (e) {
				Gunc_nerr(e, "failed to read 3bit int");
				return __LINE__;
			}
			sizes[i] |= bit << j;
		}
	}

	uint8_t sizeCounts[MAXSIZE] = {0};
	for (int i = 0; i < countGiven; ++i) {
		if (sizes[i] == 0) {
			continue;
		}
		sizeCounts[sizes[i]]++;
	}
	uint8_t *codeStarters = sizeCounts;
	uint8_t code = 0;
	uint8_t lastCount = 0;
	for (int i = 1; i < MAXSIZE; ++i) {
		code = (code + lastCount) << 1;
		lastCount = sizeCounts[i];
		codeStarters[i] = code;
	}

	uint8_t codes[ORDERLEN] = {0};
	for (int i = 0; i < ORDERLEN; ++i) {
		uint8_t sz = sizes[Shrub_MetaTree_UNORDER[i]];
		if (sz == 0) {
			continue;
		}
		codes[i] = codeStarters[sz];
		codeStarters[sz]++;
	}

	e = Shrub_Tree_init(&self->tree, self->data, Shrub_MetaTree_DATALENGTH, Shrub_MetaTree_MAXLEAVES, Shrub_MetaTree_BITSPERLEAF);
	if (e) {
		Gunc_nerr(e, "failed to init inner tree.");
		return __LINE__;
	}

	for (int i = 0; i < countGiven; ++i) {
		uint8_t x = Shrub_MetaTree_UNORDER[i];
		if (sizes[x] == 0) {
			continue;
		}
		e = Shrub_Tree_enterCode(&self->tree, codes[i], sizes[x], i);
		if (e) {
			Gunc_nerr(e, "cant enter code");
			return __LINE__;
		}
	}


	return 0;
}
