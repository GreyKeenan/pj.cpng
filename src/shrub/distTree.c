#include "./distTree.h"

#include "gunc/log.h"

#include <stddef.h>

#define GIVEN_MIN 1
#define MAXSIZE 15


int Shrub_DistTree_init(struct Shrub_DistTree *self, uint8_t *sizes, uint8_t countGiven) {

	if (self == NULL || sizes == NULL) {
		Gunc_err("null");
		return __LINE__;
	}
	if (countGiven > Shrub_DistTree_MAXLEAVES || countGiven < GIVEN_MIN) {
		Gunc_err("invalid count given: %d", countGiven);
		return __LINE__;
	}

	int e = 0;

	e = Shrub_Tree_init(&self->tree, self->data, Shrub_DistTree_DATALENGTH, Shrub_DistTree_MAXLEAVES, Shrub_DistTree_BITSPERLEAF);
	if (e) {
		Gunc_nerr(e, "failed to init tree");
		return __LINE__;
	}

	uint16_t sizeCounts[MAXSIZE + 1] = {0};
	for (int i = 0; i < countGiven; ++i) {
		if (sizes[i] > MAXSIZE) {
			Gunc_err("size too large: %d", sizes[i]);
			return __LINE__;
		}
		++sizeCounts[sizes[i]];
	}
	sizeCounts[0] = 0;

	uint16_t *codeStarters = sizeCounts;
	uint16_t code = 0;
	uint16_t lastCount = 0;
	for (int i = 0; i < MAXSIZE + 1; ++i) {
		code = (code + lastCount) << 1;
		lastCount = sizeCounts[i];
		codeStarters[i] = code;
	}

	uint16_t codes[Shrub_DistTree_MAXLEAVES] = {0};
	for (int i = 0; i < Shrub_DistTree_MAXLEAVES; ++i) {
		uint8_t sz = sizes[i];
		if (sz == 0) {
			continue;
		}
		codes[i] = codeStarters[sz];
		++codeStarters[sz];
	}

	for (int i = 0; i < countGiven; ++i) {
		if (sizes[i] == 0) {
			continue;
		}
		e = Shrub_Tree_enterCode(&self->tree, codes[i], sizes[i], i);
		if (e) {
			Gunc_nerr(e, "cant enter code");
			return __LINE__;
		}
	}

	return 0;
}
