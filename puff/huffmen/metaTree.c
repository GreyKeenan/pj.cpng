#ifdef DEBUG
#include <stdio.h>
#endif

#include "./metaTree.h"

#include "./tree.h"

#define MAXLENGTH 7

const uint8_t Puff_MetaTree_LENGTHORDER[Puff_MetaTree_MAXLEAVES] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};


static inline int Puff_MetaTree_init_validate(const uint8_t lengths[Puff_MetaTree_MAXLEAVES], uint8_t lengthsLength);
/*
	asserts lengths[any] <= MAXLENGTH
	asserts lengthsLength > 0 && lengthsLength < Puff_MetaTree_MAXLEAVES

	returns 0 on pass
*/
static inline int Puff_MetaTree_getCodes(uint8_t codesDestination[Puff_MetaTree_MAXLEAVES], const uint8_t lengths[Puff_MetaTree_MAXLEAVES], uint8_t lengthsLength);
/*
	generates tree codes corresponding to each given leaf length
		impl of technique outlined in DEFLATE spec

	assumes lengths[any] <= MAXLENGTH
	assumes lengthsLength > 0 && < Puff_MetaTree_MAXLEAVES

	codesDestination
		overwrites 'lengthsLength' bytes here
	
	returns 0 on success
*/
static inline int Puff_MetaTree_getCodes_codestarters(uint8_t destination[MAXLENGTH], const uint8_t lengths[Puff_MetaTree_MAXLEAVES], uint8_t lengthsLength);
/*
	generates the codestarters from the given codelengths

	assumes destination is 0 initialized
	assumes lengths[any] <= MAXLENGTH
	assumes lengthsLength > 0 && < Puff_MetaTree_MAXLEAVES

	destination
		writes code starters to destination
			where starter for codelength '1' is destination[0]
			since there is no codelength '0'

	returns 0 on success
*/


int Puff_MetaTree_init(struct Puff_MetaTree *self, const uint8_t lengths[Puff_MetaTree_MAXLEAVES], uint8_t lengthsLength) {

	int e = 0;

	e = Puff_MetaTree_init_validate(lengths, lengthsLength);
	if (e) return -3;

	*self = (struct Puff_MetaTree) {0};
	e = Puff_Tree_init(&self->tree, self->data, Puff_MetaTree_LENGTH, Puff_MetaTree_MAXLEAVES);
	if (e) return -1;

	uint8_t codes[Puff_MetaTree_MAXLEAVES] = {0};
	e = Puff_MetaTree_getCodes(codes, lengths, lengthsLength);
	if (e) return e;

	for (int i = 0; i < lengthsLength; ++i) {
		if (lengths[i] == 0) {
			continue;
		}
		e = Puff_Tree_enterCode(&self->tree, codes[i], lengths[i], Puff_MetaTree_LENGTHORDER[i]);
		if (e) return -2;
	}

	return 0;
}

static inline int Puff_MetaTree_init_validate(const uint8_t lengths[Puff_MetaTree_MAXLEAVES], uint8_t lengthsLength) {
	
	if (lengthsLength > Puff_MetaTree_MAXLEAVES || lengthsLength == 0) {
		return 1;
	}

	for (int i = 0; i < lengthsLength; ++i) {
		if (lengths[i] > MAXLENGTH) {
			return 2;
		}
	}

	return 0;
}


static inline int Puff_MetaTree_getCodes(uint8_t codesDestination[Puff_MetaTree_MAXLEAVES], const uint8_t lengths[Puff_MetaTree_MAXLEAVES], uint8_t lengthsLength) {

	#ifdef DEBUG
	printf("lengths: ");
	for (int i = 0; i < lengthsLength; ++i) {
		printf("%d ", lengths[i]);
	}
	printf("\n");
	#endif

	uint8_t codestarters[MAXLENGTH] = {0};
	if (Puff_MetaTree_getCodes_codestarters(codestarters, lengths, lengthsLength)) {
		return 1;
	}

	uint8_t l = 0;
	for (int i = 0; i < lengthsLength; ++i) {
		l = lengths[i];
		if (l == 0) {
			codesDestination[i] = 0;
			continue;
		}
		codesDestination[i] = codestarters[l - 1];
		codestarters[l - 1]++;
	}

	#ifdef DEBUG
	printf("codesDestination: ");
	for (int i = 0; i < lengthsLength; ++i) {
		printf("%d ", codesDestination[i]);
	}
	printf("\n");
	#endif

	return 0;
}

static inline int Puff_MetaTree_getCodes_codestarters(uint8_t destination[MAXLENGTH], const uint8_t lengths[Puff_MetaTree_MAXLEAVES], uint8_t lengthsLength) {

	//count lengths
	for (int i = 0; i < lengthsLength; ++i) {
		if (lengths[i] == 0) {
			continue;
		}

		destination[lengths[i] - 1]++;
	}
	#ifdef DEBUG
	printf("lengthCounts: ");
	for (int i = 0; i < MAXLENGTH; ++i) {
		printf("%d ", destination[i]);
	}
	printf("\n");
	#endif

	//create code starters per length
	uint8_t code = 0;
	uint8_t lCount = 0;
	for (int i = 0; i < MAXLENGTH; ++i) {
		code = (code + lCount) << 1;
		lCount = destination[i];
		destination[i] = code;
	}

	#ifdef DEBUG
	printf("codestarters: ");
	for (int i = 0; i < MAXLENGTH; ++i) {
		printf("%d ", destination[i]);
	}
	printf("\n");
	#endif

	return 0;
}
