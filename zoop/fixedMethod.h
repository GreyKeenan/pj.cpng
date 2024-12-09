#ifndef ZOOP_fixedMethod_H
#define ZOOP_fixedMethod_H

#include "./walkUntilLeaf.h"
#include "./lengthDist.h"

#include "shrub/fixedTree.h"
#include "shrub/litTree.h"

#include "gunc/log.h"
#include "gunc/bitStream.h"
#include "gunc/iByteWriter.h"
#include "gunc/iByteLooker.h"

#include <stdint.h>


static inline int Zoop_fixedMethod(struct Gunc_BitStream *bis, struct Gunc_iByteWriter *bw, struct Gunc_iByteLooker *bl) {

	int e = 0;


	e = Shrub_FixedTree_init();
	if (e) {
		Gunc_nerr(e, "failed to init FixedTree");
		return __LINE__;
	}

	const struct Shrub_Tree *tree = &Shrub_nFIXEDTREE->tree;

	uint16_t leaf = 0;
	uint16_t dist = 0;

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

	return 0;
}

#endif
