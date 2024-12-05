#include "walkLitTree.h"

#include "./walkUntilLeaf.h"

#include "shrub/tree.h"

#include "gunc/log.h"
#include "gunc/bitStream.h"
#include "gunc/iByteWriter.h"
#include "gunc/iByteLooker.h"

#define HUFFMAN_END 256

int Zoop_walkLitTree(const struct Shrub_Tree *tree, struct Gunc_BitStream *bis, struct Gunc_iByteWriter *bw, struct Gunc_iByteLooker *bl) {

	int e = 0;
	bool bit = 0;

	uint16_t leaf = 0;

	while (1) {

		e = Zoop_walkUntilLeaf(tree, bis, &leaf);
		if (e) {
			Gunc_nerr(e, "failed to go to leaf.");
		}

		if (leaf == HUFFMAN_END) {
			break;
		}

		// ...

	}

	Gunc_TODO("this");
	return 1;
}
