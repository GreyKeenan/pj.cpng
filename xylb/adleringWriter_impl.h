#ifndef XYLB_ADLERINGWRITER_H
#define XYLB_ADLERINGWRITER_H

#include "puff/iNostalgicWriter_impl.h"

struct Xylb_AdleringWriter {
	struct Puff_iNostalgicWriter nostalgicWriter;

	struct {
		uint32_t s1;
		uint32_t s2;
	} adler;
};

#endif
