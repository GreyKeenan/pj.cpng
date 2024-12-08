#ifndef Zoop_dynamicMethod_H
#define Zoop_dynamicMethod_H

#include "./walkUntilLeaf.h"
#include "./lengthDist.h"

#include "shrub/metaTree.h"
#include "shrub/litTree.h"

#include "gunc/log.h"
#include "gunc/bitStream.h"
#include "gunc/iByteWriter.h"
#include "gunc/iByteLooker.h"


static inline int Zoop_dynamicMethod(struct Gunc_BitStream *bis, struct Gunc_iByteWriter *bw, struct Gunc_iByteLooker *bl) {

	int e = 0;

	// read tree codelength-sequence-lengths
	/*
		5b: num of literal/length codeSizes - 257
		5b: num of distance codeSizes - 1
		4b: num of metaTree codeSizes - 4
			order:
			16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15

			3b ints
	*/

	for (int i = 0; i < 10; ++i) {
		e =	Gunc_BitStream_bit(bis, NULL);
		if (e) {
			Gunc_nerr(e, "failed to waste bit");
			return __LINE__;
		}
	}

	bool bit = 0;

	uint8_t metaLen = 0;
	for (int i = 0; i < 4; ++i) {
		e =	Gunc_BitStream_bit(bis, &bit);
		if (e) {
			Gunc_nerr(e, "bit read");
			return __LINE__;
		}

		metaLen |= bit << i;
	}
	metaLen += 4;

	// build metatree
	/*
		5bit metaTree codelengths in specific order
		0through15 are lengths
		16through18 are special repeat indicators
		16:
			copy prev, 3through6 times
			grab 2 more bits
		17:
			repeat '0', 3through10 times
			grab 3 more bits
		18:
			repeat '0', 11through138 times
			grab 7 more bits

		maybe create a byteStream that the build lit/dist trees use
			makes it easier to overflow repeat codes btwn them
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
