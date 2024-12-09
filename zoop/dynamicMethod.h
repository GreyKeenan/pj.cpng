#ifndef Zoop_dynamicMethod_H
#define Zoop_dynamicMethod_H

#include "./walkUntilLeaf.h"
#include "./lengthDist.h"

#include "shrub/litTree.h"
#include "shrub/metaTree.h"
#include "shrub/dynaTree.h"
#include "shrub/distTree.h"

#include "gunc/log.h"
#include "gunc/bitStream.h"
#include "gunc/iByteWriter.h"
#include "gunc/iByteLooker.h"

#define LITLEN_MIN 257
#define DISTLEN_MIN 1
#define METALEN_MIN 4


static inline int Zoop_dynamicMethod(struct Gunc_BitStream *bis, struct Gunc_iByteWriter *bw, struct Gunc_iByteLooker *bl) {

	int e = 0;

	// read tree codelength-sequence-lengths
	/*
		5b: num of literal/length codeSizes - 257
		5b: num of distance codeSizes - 1
		4b: num of metaTree codeSizes - 4
	*/

	bool bit = 0;

	uint16_t litLen = 0;
	for (int i = 0; i < 5; ++i) {
		e =	Gunc_BitStream_bit(bis, &bit);
		if (e) {
			Gunc_nerr(e, "failed to read litLen");
			return __LINE__;
		}
		litLen |= bit << i;
	}
	litLen += LITLEN_MIN;

	uint8_t distLen = 0;
	for (int i = 0; i < 5; ++i) {
		e =	Gunc_BitStream_bit(bis, &bit);
		if (e) {
			Gunc_nerr(e, "failed to waste bit");
			return __LINE__;
		}
		distLen = bit << i;
	}
	distLen += DISTLEN_MIN;

	uint8_t metaLen = 0;
	for (int i = 0; i < 4; ++i) {
		e =	Gunc_BitStream_bit(bis, &bit);
		if (e) {
			Gunc_nerr(e, "bit read");
			return __LINE__;
		}

		metaLen |= bit << i;
	}
	metaLen += METALEN_MIN;

	Gunc_say("litLen: %d distLen: %d metaLen: %d", litLen, distLen, metaLen);

	// build metatree
	/*
		3bit metaTree codesizes in specific order
	*/
	struct Shrub_MetaTree mTree = {0};

	e = Shrub_MetaTree_init(&mTree, bis, metaLen);
	if (e) {
		Gunc_nerr(e, "failed to init meta tree.");
		return __LINE__;
	}
	Gunc_say("metaTree initialized!");

	// build litTree
	/*
		metaTree codes in specific order
	*/
	// build distTree
	/*
		metaTree codes in specific order
	*/

	uint8_t sizes[Shrub_LitTree_MAXLEAVES + Shrub_DistTree_MAXLEAVES] = {0};
		//store adjacent to simplify overlapping metaTree repeats
	uint8_t *litSizes = sizes;
	uint8_t *distSizes = sizes + litLen;

	uint16_t leaf = 0;
	uint16_t repeatFor = 0;
	uint8_t moreBits = 0;

	for (uint16_t i = 0; i < litLen + distLen; ++i) {
		e = Zoop_walkUntilLeaf(&mTree.tree, bis, &leaf);
		if (e) {
			Gunc_nerr(e, "failed to walk metaTree (%d)th", i);
			return __LINE__;
		}

		if (leaf < 16) {
			sizes[i] = leaf;
			continue;
		}
		switch (leaf) {
			case 16:
				// 2 bits, 3-6 (prev)
				if (i == 0) {
					Gunc_nerr(e, "First leaf (%d). Cannot repeat.", i);
					return __LINE__;
				}

				repeatFor = 3;
				moreBits = 2;

				break;
			case 17:
				// 3 bits, 3-10 (0)
				repeatFor = 3 - 1;
				moreBits = 3;

				sizes[i] = 0;
				++i;
				if (i == 0) {
					Gunc_err("overflowed i");
					return __LINE__;
				}
				break;
			case 18:
				// 7 bits, 11-138 (0)
				repeatFor = 11 - 1;
				moreBits = 7;

				sizes[i] = 0;
				++i;
				if (i == 0) {
					Gunc_err("overflowed i");
					return __LINE__;
				}
				break;
			default:
				Gunc_err("unrecognized mTree leaf: %d (%d)th", leaf, i);
				return __LINE__;
		}

		for (int j = 0; j < moreBits; ++j) {
			e = Gunc_BitStream_bit(bis, &bit);
			if (e) {
				Gunc_nerr(e, "failed extra bit for (%d).", leaf);
				return __LINE__;
			}
			repeatFor += bit << j;
		}

		for (int j = 0; j < repeatFor; ++j) {
			sizes[i] = sizes[i - 1];
			++i;
			if (i == 0) {
				Gunc_nerr(e, "overflowed i");
				return __LINE__;
			}
		}
		--i;

	}

	// actually init the trees using the codeSize sequences

	struct Shrub_LitTree lTree = {0};
	e = Shrub_DynaTree_init(&lTree, litSizes, litLen);
	if (e) {
		Gunc_nerr(e, "failed to init dynamic lit tree");
		return __LINE__;
	}

	struct Shrub_DistTree dTree = {0};
	e = Shrub_DistTree_init(&dTree);
	if (e) {
		Gunc_nerr(e, "failed to init distTree");
		return __LINE__;
	}

	// ...



	

	// read literal sequence
	/*
	while (1) {

		e = Zoop_walkUntilLeaf(tree, bis, &leaf);
		if (e) {
			Gunc_nerr(e, "failed to go to leaf.");
			return __LINE__;
		}

		if (leaf == Zoop_HUFFMAN_END) {
			break;
		}

		if (leaf < 256) {
			e = Gunc_iByteWriter_give(bw, leaf);
			if (e) {
				Gunc_nerr(e, "failed to write: %d", leaf);
				return __LINE__;
			}
			continue;
		}

		e = Zoop_getLength(bis, &leaf);
		if (e) {
			Gunc_nerr(e, "unable to decipher length symbol: ", leaf);
			return __LINE__;
		}

		e = Zoop_getFixedDist(bis, &dist);
		if (e) {
			Gunc_nerr(e, "failed to read length");
			return __LINE__;
		}


		e = Zoop_nostalgize(bw, bl, leaf, dist);
		if (e) {
			Gunc_nerr(e, "failed to nostalgize");
			return __LINE__;
		}
	}
	*/

	return -444;
}

#endif
