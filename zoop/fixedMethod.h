#ifndef ZOOP_fixedMethod_H
#define ZOOP_fixedMethod_H

#include "shrub/fixedTree.h"
#include "shrub/tree.h"

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


	Gunc_TODO("this");
	return 1;
}

#endif
