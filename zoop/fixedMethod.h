#ifndef ZOOP_fixedMethod_H
#define ZOOP_fixedMethod_H

#include "./walkLitTree.h"

#include "shrub/fixedTree.h"
#include "shrub/litTree.h"

#include "gunc/log.h"
#include "gunc/bitStream.h"
#include "gunc/iByteWriter.h"
#include "gunc/iByteLooker.h"

#include <stdint.h>

static inline int Zoop_fixedMethod(struct Gunc_BitStream *bis, struct Gunc_iByteWriter *bw, struct Gunc_iByteLooker *bl) {

	int e = 0;
	uint8_t byte = 0;


	e = Shrub_FixedTree_init();
	if (e) {
		Gunc_nerr(e, "failed to init FixedTree");
		return __LINE__;
	}


	e = Zoop_walkLitTree(&Shrub_nFIXEDTREE->tree, bis, bw, bl);
	if (e) {
		Gunc_nerr(e, "failed to walk");
		return __LINE__;
	}

	return 0;
}

#endif
