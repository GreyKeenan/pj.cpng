
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "gunc/log.h"
#include "gunc/iByteStream.h"
#include "gunc/iByteWriter.h"
#include "gunc/files.h"
#include "gunc/byteBalloon.h"
#include "gunc/sequence.h"
#include "gunc/iByteLooker.h"

#include "whine/stripng.h"
#include "whine/image.h"
#include "whine/nofilter.h"
#include "whine/pixie.h"

#include "zoop/decompress.h"

static inline int unwrap_png(const char *path, uint8_t **dataDest, uint32_t *lengthDest, struct Whine_Image *image);
static inline int decompress(uint8_t *zlibData, uint32_t zlibDataLength, uint8_t **dataDest, uint32_t *lengthDest);
static inline int remove_filters(struct Whine_Image *image, uint8_t *decompressedData, uint32_t length);
static inline int show(struct Whine_Image image);

int main(int argc, char **argv) {
	Gunc_title("Starting Program: %s", argv[0]);

	int e = 0;

	struct Whine_Image image = {0};

	uint8_t *hnZlibData = NULL;
	uint32_t zlibDataLength = 0;

	uint8_t *hnDecompressedData = NULL;
	uint32_t decompressedDataLength = 0;

	if (argc < 2) {
		Gunc_err("missing png file path");
		goto fin;
	}
	Gunc_say("filepath: %s", argv[1]);


	e = unwrap_png(argv[1], &hnZlibData, &zlibDataLength, &image);
	if (e) {
		Gunc_nerr(e, "failed to unwrap png.");
		goto fin;
	}
	Gunc_say("data: %p length: %d", hnZlibData, zlibDataLength);

	e = decompress(hnZlibData, zlibDataLength, &hnDecompressedData, &decompressedDataLength);
	if (e) {
		Gunc_nerr(e, "failed to decompress zlib data.");
		goto fin;
	}
	Gunc_say("decompressed data: %p length: %d", hnDecompressedData, decompressedDataLength);

	free(hnZlibData);
	hnZlibData = NULL;
	zlibDataLength = 0;


	e = remove_filters(&image, hnDecompressedData, decompressedDataLength);
	if (e) {
		Gunc_nerr(e, "failed to remove scanline filters");
		goto fin;
	}

	free(hnDecompressedData);
	hnDecompressedData = NULL;
	decompressedDataLength = 0;

	e = show(image);

	fin:
	if (hnZlibData != NULL) {
		free(hnZlibData);
	}
	if (hnDecompressedData != NULL) {
		free(hnDecompressedData);
	}
	if (image.nScanlineData != NULL) {
		free(image.nScanlineData);
	}

	Gunc_title("Program endpoint reached.");
	return 0;
}

static inline int unwrap_png(const char *path, uint8_t **dataDest, uint32_t *lengthDest, struct Whine_Image *image) {

	int e = 0;
	int r = 0;

	struct Gunc_iByteStream bs = {0};
	struct Gunc_iByteWriter bw = {0};

	FILE *f = NULL;
	struct Gunc_ByteBalloon bb = {0};


	Gunc_title("Whine_stripng Setup");

	e = Gunc_file_as_iByteStream(&f, &bs, path);
	if (e) {
		Gunc_nerr(e, "Failed to open file: %s", path);
		r = __LINE__;
		goto fin;
	}

	e = Gunc_ByteBalloon_init(&bb, 1024);
	if (e) {
		Gunc_nerr(e, "failed to init byteBalloon %p", &bb);
		r = __LINE__;
		goto fin;
	}
	e = Gunc_ByteBalloon_as_iByteWriter(&bb, &bw);
	if (e) {
		Gunc_nerr(e, "failed to create iByteWriter from byteBalloon");
		r = __LINE__;
		goto fin;
	}


	Gunc_title("Stripping PNG");

	e = Whine_stripng(image, &bs, &bw);
	if (e) {
		Gunc_nerr(e, "failed to extract zlib data from png");
		r = __LINE__;
		goto fin;
	}


	Gunc_title("Whine Closeout");

	e = Gunc_ByteBalloon_trim(&bb);
	if (e) {
		Gunc_nerr(e, "failed to trim byteBalloon.");
		r = __LINE__;
		goto fin;
	}

	*dataDest = bb.hData;
	bb.hData = NULL;
	*lengthDest = bb.length;


	fin:
	if (bb.hData != NULL) {
		free(bb.hData);
	}
	if (f != NULL) {
		e = fclose(f);
		if (e) {
			Gunc_nerr(e, "failed to fclose() file");
			r = __LINE__;
		}
	}
	return r;
}

static inline int decompress(uint8_t *zlibData, uint32_t zlibDataLength, uint8_t **dataDest, uint32_t *lengthDest) {

	int e = 0;

	Gunc_title("Zoop setup");

	struct Gunc_ByteBalloon bb = {0};
	struct Gunc_iByteWriter bw = {0};
	struct Gunc_iByteLooker bl = {0};

	e = Gunc_ByteBalloon_init(&bb, 1024);
	if (e) {
		Gunc_nerr(e, "failed to init byteBalloon for zoop.");
		return __LINE__;
	}
	e = Gunc_ByteBalloon_as_iByteWriter(&bb, &bw);
	if (e) {
		Gunc_nerr(e, "failed to create iByteWriter from byteBalloon for zoop.");
		return __LINE__;
	}
	e = Gunc_ByteBalloon_as_iByteLooker(&bb, &bl);
	if (e) {
		Gunc_nerr(e, "failed to create iByteLooker from byteBalloon");
		return __LINE__;
	}

	struct Gunc_Sequence seq = {0};
	struct Gunc_iByteStream bs = {0};

	e = Gunc_Sequence_init(&seq, zlibData, zlibDataLength, sizeof(*zlibData));
	if (e) {
		Gunc_nerr(e, "failed to init Sequence from bb_whine");
		return __LINE__;
	}
	e = Gunc_Sequence_as_iByteStream(&seq, &bs);
	if (e) {
		Gunc_nerr(e, "failed to create iByteStream from Sequence");
		return __LINE__;
	}

	
	Gunc_title("Decompressing Zlib Bytes");

	e = Zoop_decompress(&bs, &bw, &bl);
	if (e) {
		Gunc_nerr(e, "failed to zoop the zoopidee zoop");
		return __LINE__;
	}


	Gunc_title("Zoop Closeout");

	*dataDest = bb.hData;
	*lengthDest = bb.length;

	e = Gunc_ByteBalloon_trim(&bb);
	if (e) {
		Gunc_nerr(e, "failed to trim byteBalloon. Returned data may have trailing bytes.");
		return __LINE__;
	}

	*dataDest = bb.hData;
	*lengthDest = bb.length;

	return 0;
}

static inline int remove_filters(struct Whine_Image *image, uint8_t *decompressedData, uint32_t length) {

	int e = 0;

	Gunc_title("Defiltering setup");

	struct Gunc_Sequence seq = {0};
	struct Gunc_iByteStream bs = {0};

	e = Gunc_Sequence_init(&seq, decompressedData, length, sizeof(*decompressedData));
	if (e) {
		Gunc_nerr(e, "failed to init Sequence");
		return __LINE__;
	}
	e = Gunc_Sequence_as_iByteStream(&seq, &bs);
	if (e) {
		Gunc_nerr(e, "failed to init bs from seq");
		return __LINE__;
	}


	Gunc_title("nofiltering");
	
	e = Whine_nofilter(image, &bs);
	if (e) {
		Gunc_nerr(e, "nofiltering failed");
		return __LINE__;
	}

	Gunc_say("scanline image data obtained & defiltered: %p", image->nScanlineData);

	return 0;
}

static inline int show(struct Whine_Image image) {
	
	int e = 0;

	struct Whine_Pixie pix = {0};

	Gunc_title("Creating Pixie");

	e = Whine_Pixie_init(&pix, image);
	if (e) {
		Gunc_nerr(e, "failed to init pixie");
		return __LINE__;
	}

	uint32_t n = 0;
	e = Whine_Pixie_nextPixel(&pix, &n);
	if (e) {
		Gunc_nerr(e, "failed to read pixel");
		return __LINE__;
	}
	Gunc_say("pixel 0: 0x%x", n);

	return 0;
}


#include "gunc/log.c"
#include "gunc/files.c"
#include "gunc/byteBalloon.c"
#include "gunc/sequence.c"
#include "gunc/bitStream.c"
#include "gunc/byteSeq64.c"

#include "whine/all.c"
#include "zoop/all.c"
