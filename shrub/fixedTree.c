#include "./fixedTree.h"

#include "./litTree.h"

#include "gunc/log.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define NUM 4
#define CODESTARTERS {0x30, 400, 0, 0xc0}
#define CODELENGTHS {8, 9, 7, 8}
#define CODELENGTHCOUNTS {144, 112, 24, 8}
const uint8_t Shrub_FixedTree_codeLengths[NUM] = CODELENGTHS;
const uint8_t Shrub_FixedTree_codeLengthCounts[NUM] = CODELENGTHCOUNTS;

struct Shrub_LitTree Shrub_FixedTree_self = {0};
const struct Shrub_LitTree *Shrub_nFIXEDTREE = NULL;

int Shrub_FixedTree_init() {
	if (Shrub_nFIXEDTREE != NULL) {
		return 0;
	}


	int e = 0;
	struct Shrub_LitTree worktree = {0};

	e = Shrub_LitTree_init(&worktree);
	if (e) {
		Gunc_nerr(e, "failed to init litTree");
		return 2;
	}

	uint16_t codeStarters[NUM] = CODESTARTERS;
	uint16_t offset = 0;
	for (int j = 0; j < NUM; ++j) {
		uint8_t length = Shrub_FixedTree_codeLengths[j];
		for (int i = 0; i < Shrub_FixedTree_codeLengthCounts[j]; ++i) {
			e = Shrub_Tree_enterCode(&worktree.tree, codeStarters[j], length, offset + i);
			if (e) {
				Gunc_nerr(e, "failed to enter code.");
				return 3;
			}

			codeStarters[j]++;
		}
		offset += Shrub_FixedTree_codeLengthCounts[j];
	}


	if (Shrub_nFIXEDTREE != NULL) {
		return 0;
	}
	Shrub_FixedTree_self = worktree;
	Shrub_FixedTree_self.tree.data = Shrub_FixedTree_self.data;

	Shrub_nFIXEDTREE = &Shrub_FixedTree_self;

	return 0;
}
