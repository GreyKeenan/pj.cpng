#include "./walkUntilLeaf.h"

#include "shrub/tree.h"

#include "gunc/log.h"
#include "gunc/bitStream.h"

int Zoop_walkUntilLeaf(const struct Shrub_Tree *tree, struct Gunc_BitStream *bis, uint16_t *nDestination) {

	int e = 0;
	bool bit = 0;

	uint16_t currentIndex = Shrub_Tree_ROOT;
	uint16_t child = 0;

	/*
	#ifdef DEBUG
	uint16_t Dcode = 0;
	uint16_t DcodeLength = 0;
	#endif
	*/

	while (1) {
		e = Gunc_BitStream_bit(bis, &bit);
		if (e) {
			Gunc_nerr(e, "bitstream err");
			return __LINE__;
		}

		/*
		#ifdef DEBUG
		Dcode <<= 1;
		Dcode |= bit;
		DcodeLength ++;
		#endif
		*/

		e = Shrub_Tree_walk(tree, bit, currentIndex, &child);
		if (e) {
			Gunc_nerr(e, "walk failure");
			return __LINE__;
		}
		if (!(child & Shrub_Tree_LEAF)) {
			currentIndex = child >> 1;
			continue;
		}

		child >>= 1;

		#ifdef DEBUG_Zoop_walkUntilLeaf
		Gunc_say("hit leaf: %d", child);
		#endif

		break;
	}

	if (nDestination != NULL) {
		*nDestination = child;
	}

	return 0;
}
