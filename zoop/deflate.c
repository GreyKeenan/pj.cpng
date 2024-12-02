#include "./deflate.h"

#include "gunc/log.h"
#include "gunc/bitStream.h"
#include "gunc/iByteWriter.h"
#include "gunc/iByteLooker.h"

int Zoop_deflate(struct Gunc_BitStream *bis, struct Gunc_iByteWriter *bw, struct Gunc_iByteLooker *bl) {

	int e = 0;
	bool bit = 0;

	bool isLastBlock = false;
	uint8_t compressionType = 0;

	do {
		e = Gunc_BitStream_bit(bis, &isLastBlock);
		if (e) {
			Gunc_nerr(e, "failed to fetch bit");
			return __LINE__;
		}

		e = Gunc_BitStream_bit(bis, &bit);
		if (e) {
			Gunc_nerr(e, "failed to fetch bit ct0");
			return __LINE__;
		}
		compressionType = bit;
		e = Gunc_BitStream_bit(bis, &bit);
		if (e) {
			Gunc_nerr(e, "failed to fetch bit ct1");
			return __LINE__;
		}
		compressionType |= bit << 1;

		switch (compressionType) {
			case 0:
				Gunc_TODO("uncompressed compression type");
				break;
			case 1:
				Gunc_TODO("fixed compression type");
				break;
			case 2:
				Gunc_TODO("dynamic compression type");
				break;
			case 3:
				Gunc_err("reserved compression type (%d).", compressionType);
				return __LINE__;
			default:
				Gunc_err("unrecognized compressionType. How did this even happen? (%d)", compressionType);
				return __LINE__;
		}

	} while (!isLastBlock);

	return 0;
}
