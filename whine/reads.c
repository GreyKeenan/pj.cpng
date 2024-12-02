#include "./reads.h"

#include <stddef.h>

#include "gunc/log.h"
#include "gunc/iByteStream.h"

int Whine_read_int32(struct Gunc_iByteStream *bs, uint32_t *nDestination) {

	int e = 0;
	uint8_t b = 0;
	uint32_t n = 0;

	for (int i = 0; i < 4; ++i) {
		e = Gunc_iByteStream_next(bs, &b);
		if (e) {
			Gunc_nerr(e, "unable to read byte");
			return 1;
		}
		n = (n << 8) | b;
	}

	if (n & 0x80000000) {
		Gunc_err("int32 exceeds PNG cap with: 0x%x", n);
		return 2;
	}

	if (nDestination != NULL) {
		*nDestination = n;
	}

	return 0;
}
