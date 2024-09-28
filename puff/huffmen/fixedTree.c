#include <stdio.h>

#include "./fixedTree.h"

#include "./literalTree_impl.h"

#include "./tree.h"

#include <stdint.h>

#define NUM 4
#define CODESTARTERS {0x30, 400, 0, 0xc0}
#define CODELENGTHS {8, 9, 7, 8}
#define CODELENGTHCOUNTS {144, 112, 24, 8}
const uint8_t Puff_FixedTree_CodeLengths[NUM] = CODELENGTHS;
const uint8_t Puff_FixedTree_CodeLengthCounts[NUM] = CODELENGTHCOUNTS;


static inline int Puff_FixedTree_enterCode(struct Puff_LiteralTree *self, uint16_t code, uint8_t codeLength, uint16_t value);

int Puff_FixedTree_init(struct Puff_LiteralTree *self) {

	int e = 0;

	*self = (struct Puff_LiteralTree) {0};

	e = Puff_Tree_init(&self->tree, self->data, Puff_LiteralTree_LENGTH, Puff_LiteralTree_MAXLEAVES);
	if (e) return 1;

	// populate
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	uint16_t codeStarters[NUM] = CODESTARTERS;
	uint16_t offset = 0;

	for (int j = 0; j < NUM; ++j) {
		uint8_t length = Puff_FixedTree_CodeLengths[j];
		for (int i = 0; i < Puff_FixedTree_CodeLengthCounts[j]; ++i) {
			e = Puff_FixedTree_enterCode(self, codeStarters[j], length, offset + i);
			if (e) return e;

			codeStarters[j]++;
		}
		offset += Puff_FixedTree_CodeLengthCounts[j];
	}

	return 0;
}


static inline int Puff_FixedTree_enterCode(struct Puff_LiteralTree *self, uint16_t code, uint8_t codeLength, uint16_t value) {

	//printf("entering code: 0x%x(%d) : %d\n", code, codeLength, value);

	uint16_t nodeIndex = Puff_Tree_ROOT;
	uint16_t child = 0;

	for (int i = 1 << (codeLength - 1); i != 0; i >>= 1) {
		if (i == 1) {
			if (Puff_Tree_growLeaf(&self->tree, nodeIndex, code & i, value)) {
				return 4;
			}
			continue;
		}
		switch (Puff_Tree_walk(&self->tree, nodeIndex, code & i, &child)) {
			case Puff_Tree_ISNODE:
				nodeIndex = child;
				break;
			case Puff_Tree_HALT:
				if (Puff_Tree_birthNode(&self->tree, nodeIndex, code & i, &child)) {
					return 3;
				}
				nodeIndex = child;
				break;
			default:
				return 2;
		}
	}

	return 0;
}
