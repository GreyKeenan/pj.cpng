#include "walkLitTree.h"

#include "./walkUntilLeaf.h"
#include "./lengthDist.h"

#include "shrub/tree.h"

#include "gunc/log.h"
#include "gunc/bitStream.h"
#include "gunc/iByteWriter.h"
#include "gunc/iByteLooker.h"

#define HUFFMAN_END 256

// TODO because dist tree is diff, need separate funcs for Fixed & Dynamic
int Zoop_walkLitTree(const struct Shrub_Tree *tree, struct Gunc_BitStream *bis, struct Gunc_iByteWriter *bw, struct Gunc_iByteLooker *bl) {

	int e = 0;
	bool bit = 0;

	uint16_t leaf = 0;
	uint16_t dist = 0;

	while (1) {

		e = Zoop_walkUntilLeaf(tree, bis, &leaf);
		if (e) {
			Gunc_nerr(e, "failed to go to leaf.");
			return __LINE__;
		}

		if (leaf == HUFFMAN_END) {
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
