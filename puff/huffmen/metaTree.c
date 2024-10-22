#ifdef DEBUG
#include <stdio.h>
#endif

#include "./metaTree.h"

#include "./tree.h"

#define MAX_CODELENGTH 15

const uint8_t Puff_MetaTree_LENGTHSORDER[Puff_MetaTree_MAXLEAVES] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};


static inline int Puff_MetaTree_getCodes(uint8_t *codesDestination, const uint8_t lengths[Puff_MetaTree_MAXLEAVES], uint8_t lengthsLength);


int Puff_MetaTree_init(struct Puff_MetaTree *self, const uint8_t lengths[Puff_MetaTree_MAXLEAVES], uint8_t lengthsLength) {

	int e = 0;

	*self = (struct Puff_MetaTree) {0};
	e = Puff_Tree_init(&self->tree, self->data, Puff_MetaTree_LENGTH, Puff_MetaTree_MAXLEAVES);
	if (e) return -1;

	uint8_t codes[Puff_MetaTree_MAXLEAVES] = {0};
	e = Puff_MetaTree_getCodes(codes, lengths, lengthsLength);
	if (e) return e;

	for (int i = 0; i < lengthsLength; ++i) {
		e = Puff_Tree_enterCode(&self->tree, codes[i], lengths[i], Puff_MetaTree_LENGTHSORDER[i]);
		if (e) return -2;
	}

	return 0;
}


static inline int Puff_MetaTree_getCodes(uint8_t *codesDestination, const uint8_t lengths[Puff_MetaTree_MAXLEAVES], uint8_t lengthsLength) {

	#ifdef DEBUG
	printf("lengths: ");
	for (int i = 0; i < lengthsLength; ++i) {
		printf("%d ", lengths[i]);
	}
	printf("\n");
	#endif

	//uint8_t lengthCounts[Puff_MetaTree_MAXLEAVES] = {0};
	for (int i = 0; i < lengthsLength; ++i) {
		//lengthCounts[lengths[i]]++;
		codesDestination[lengths[i]]++;
	}
	//lengthCounts[0] = 0;
	codesDestination[0] = 0;

	#ifdef DEBUG
	printf("lengthCounts: ");
	for (int i = 0; i < lengthsLength; ++i) {
		//printf("%d ", lengthCounts[i]);
		printf("%d ", codesDestination[i]);
	}
	printf("\n");
	#endif

	uint8_t mincodes[MAX_CODELENGTH] = {0};
	for (int i = 1; i < MAX_CODELENGTH; ++i) {
		//mincodes[i] = (mincodes[i-1] + lengthCounts[i-1]) << 1;
		mincodes[i] = (mincodes[i-1] + codesDestination[i-1]) << 1;
	}

	#ifdef DEBUG
	printf("mincodes: ");
	for (int i = 0; i < MAX_CODELENGTH; ++i) {
		printf("%d ", mincodes[i]);
	}
	printf("\n");
	#endif

	uint8_t l = 0;
	for (int i = 0; i < lengthsLength; ++i) {
		l = lengths[i];
		if (l != 0) {
			codesDestination[i] = mincodes[l];
			mincodes[l]++;
		}
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
