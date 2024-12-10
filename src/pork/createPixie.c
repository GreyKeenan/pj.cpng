#include "./createPixie.h"

#include "gunc/log.h"

#include "gunc/files.h"

#include "gunc/iByteStream.h"
#include "gunc/iByteWriter.h"
#include "gunc/iByteLooker.h"
#include "gunc/bitStream.h"
#include "gunc/byteBalloon.h"

#include "whine/pixie.h"
#include "whine/stripng.h"
#include "whine/nofilter.h"

#include "zoop/decompress.h"

#include <stdlib.h>
#include <stdio.h>

#define BBSIZE 1024

static inline int Pork_unwrapZlib(struct Gunc_iByteStream *bys, struct Whine_Image *image);
static inline int Pork_decompress(struct Whine_Image *image);
static inline int Pork_defilter(struct Whine_Image *destination);


int Pork_createPixie(const char *path, struct Whine_Pixie *destination) {

	int e = 0;

	FILE *f = NULL;

	struct Whine_Image image = {0};

	struct Gunc_iByteStream bys = {0};

	f = fopen(path, "rb");
	if (f == NULL) {
		Gunc_err("FAILED TO OPEN FILE: %s", path);
		e = __LINE__;
		goto fin;
	}
	e = Gunc_file_as_iByteStream(f, &bys);
	if (e) {
		Gunc_nerr(e, "failed to init iByteStream from file");
		e = __LINE__;
		goto fin;
	}

	e = Pork_unwrapZlib(&bys, &image);
	if (e) {
		Gunc_nerr(e, "failed to unwrap zlib data");
		e = __LINE__;
		goto fin;
	}

	e = fclose(f);
	f = NULL;
	if (e) {
		Gunc_nerr(e, "FAILED TO CLOSE FILE %s", path);
		e = -1;
		goto fin;
	}


	e = Pork_decompress(&image);
	if (e) {
		Gunc_nerr(e, "failed to decompress");
		e = __LINE__;
		goto fin;
	}


	e = Pork_defilter(&image);
	if (e) {
		Gunc_nerr(e, "failed to defilter");
		e = __LINE__;
		goto fin;
	}


	e = Whine_Pixie_init(destination, image);
	if (e) {
		Gunc_nerr(e, "failed to init pixie");
		e = __LINE__;
		goto fin;
	}
	image = (struct Whine_Image){0};



	fin:

	Whine_Image_filicide(&image);

	if (f != NULL) {
		e = fclose(f);
		if (e) {
			Gunc_nerr(e, "FAILED TO CLOSE FILE %s", path);
			e = -1;
		}
		f = NULL;
	}

	return e;
}

static inline int Pork_unwrapZlib(struct Gunc_iByteStream *bys, struct Whine_Image *image) {

	int e = 0;

	struct Gunc_ByteBalloon bb = {0};
	struct Gunc_iByteWriter bw = {0};
	
	e = Gunc_ByteBalloon_init(&bb, BBSIZE);
	if (e) {
		Gunc_nerr(e, "failed to init ByteBalloon");
		e = __LINE__;
		goto fin;
	}
	e = Gunc_ByteBalloon_as_iByteWriter(&bb, &bw);
	if (e) {
		Gunc_nerr(e, "failed to init iByteWriter");
		e = __LINE__;
		goto fin;
	}

	e = Whine_stripng(image, bys, &bw);
	if (e) {
		Gunc_nerr(e, "failed to extract zlib from png");
		e = __LINE__;
		goto fin;
	}

	e = Gunc_ByteBalloon_trim(&bb);
	if (e) {
		Gunc_nerr(e, "failed to trim byteBalloon");
		e = __LINE__;
		goto fin;
	}

	Whine_Image_setData(image, bb.hData, Whine_Image_COMPRESSED);
	image->imageDataLength = bb.length;
	bb.hData = NULL;



	fin:

	if (bb.hData != NULL) {
		free(bb.hData);
		bb.hData = NULL;
	}

	return e;
}

static inline int Pork_decompress(struct Whine_Image *image) {

	int e = 0;

	struct Gunc_ByteBalloon bb = {0};
	struct Gunc_iByteWriter bw = {0};
	struct Gunc_iByteLooker bl = {0};
	struct Gunc_iByteStream bys = {0};

	e = Gunc_ByteBalloon_init(&bb, BBSIZE);
	if (e) {
		Gunc_nerr(e, "failed to init ByteBalloon");
		e = __LINE__;
		goto fin;
	}
	e = Gunc_ByteBalloon_as_iByteWriter(&bb, &bw);
	if (e) {
		Gunc_nerr(e, "failed to init iByteWriter");
		e = __LINE__;
		goto fin;
	}
	e = Gunc_ByteBalloon_as_iByteLooker(&bb, &bl);
	if (e) {
		Gunc_nerr(e, "failed to create iByteLooker from byteBalloon");
		e = __LINE__;
		goto fin;
	}

	if (image->hnImageData == NULL) {
		Gunc_err("null image data");
		e = __LINE__;
		goto fin;
	}
	struct Gunc_ByteSeq64 seq = {
		.data = image->hnImageData,
		.length = image->imageDataLength
	};
	e = Gunc_ByteSeq64_as_iByteStream(&seq, &bys);
	if (e) {
		Gunc_nerr(e, "failed to init byte stream from seq64");
		e = __LINE__;
		goto fin;
	}

	e = Zoop_decompress(&bys, &bw, &bl);
	if (e) {
		Gunc_nerr(e, "failed to decompress data");
		e = __LINE__;
		goto fin;
	}

	e = Gunc_ByteBalloon_trim(&bb);
	if (e) {
		Gunc_nerr(e, "failed to trim byteBalloon");
		e = __LINE__;
		goto fin;
	}

	free(image->hnImageData);
	Whine_Image_setData(image, bb.hData, Whine_Image_FILTERED);
	image->imageDataLength = bb.length;
	bb.hData = NULL;



	fin:

	if (bb.hData != NULL) {
		free(bb.hData);
		bb.hData = NULL;
	}

	return e;
}

static inline int Pork_defilter(struct Whine_Image *destination) {

	int e = 0;

	if (destination == NULL || destination->hnImageData == NULL) {
		Gunc_nerr(e, "null input");
		return __LINE__;
	}

	struct Whine_Image image = {
		.header = destination->header
	};

	struct Gunc_iByteStream bys = {0};
	struct Gunc_ByteSeq64 seq = {
		.data = destination->hnImageData,
		.length = destination->imageDataLength
	};
	e = Gunc_ByteSeq64_as_iByteStream(&seq, &bys);
	if (e) {
		Gunc_nerr(e, "failed to init byte stream from seq64");
		e = __LINE__;
		goto fin;
	}

	e = Whine_nofilter(&image, &bys);
	if (e) {
		Gunc_nerr(e, "failed to undo filters");
		e = __LINE__;
		goto fin;
	}

	free(destination->hnImageData);
	Whine_Image_setData(destination, image.hnImageData, image.imageDataStatus);
	destination->imageDataLength = image.imageDataLength;
	image = (struct Whine_Image){0};



	fin:

	Whine_Image_filicide(&image);

	return e;
}
