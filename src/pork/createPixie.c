#include "./createPixie.h"

#include "gunc/log.h"

#include "gunc/files.h"

#include "gunc/iByteStream.h"
#include "gunc/iByteWriter.h"
#include "gunc/iByteLooker.h"
#include "gunc/bitStream.h"
#include "gunc/byteBalloon.h"

#include "gunc/byteBalloon64.h"
#include "gunc/byteUence64.h"
#include "gunc/byteArr64.h"

#include "whine/pixie.h"
#include "whine/stripng.h"
#include "whine/nofilter.h"

#include "whine/easel.h"
#include "whine/canvas.h"

#include "zoop/decompress.h"

#include <stdlib.h>
#include <stdio.h>

#define BBSIZE 1024

static inline int Pork_decompress(struct Whine_Easel *easel);
static inline int Pork_defilter(struct Whine_Easel *destination);

int Pork_createPixie(const char *path, struct Whine_Pixie *destination) {

	int e = 0;

	FILE *f = NULL;

	struct Gunc_iByteStream bys = {0};
	struct Gunc_iByteWriter bw = {0};

	struct Whine_Easel easel = {0};
	struct Whine_Canvas canvas = {0};
	struct Whine_Canvas decompressedCanvas = {0};

	struct Gunc_ByteBalloon64 bb = {0};

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

	e = Gunc_ByteBalloon64_init(&bb, &canvas.image, BBSIZE);
	if (e) {
		Gunc_nerr(e, "failed to init bb");
		e = __LINE__;
		goto fin;
	}
	e = Gunc_ByteBalloon64_as_iByteWriter(&bb, &bw);
	if (e) {
		Gunc_nerr(e, "failed to init bw from bb");
		e = __LINE__;
		goto fin;
	}

	e = Whine_stripng(&bys, &bw, &easel);
	if (e) {
		Gunc_nerr(e, "failed to unwrap zlib");
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

	e = Gunc_ByteBalloon64_trim(&bb);
	if (e) {
		Gunc_nerr(e, "failed to trim bb");
		e = -1;
		goto fin;
	}
	canvas.status = Whine_Canvas_COMPRESSED;

	//

	e = Gunc_ByteBalloon64_init(&bb, &decompressedCanvas.image, BBSIZE);
	if (e) {
		Gunc_nerr(e, "failed to init bb");
		e = __LINE__;
		goto fin;
	}
	e = Gunc_ByteBalloon64_as_iByteWriter(&bb, &bw);
	if (e) {
		Gunc_nerr(e, "failed to init bw from bb");
		e = __LINE__;
		goto fin;
	}

	struct Gunc_ByteUence64 uence = { .arr = &canvas.image };
	e = Gunc_ByteUence64_as_iByteStream(&uence, &bys);
	if (e) {
		Gunc_nerr(e, "failed to init bs from uence");
		e = __LINE__;
		goto fin;
	}

	struct Gunc_iByteLooker bl = {0};
	e = Gunc_ByteArr64_as_iByteLooker(&decompressedCanvas.image, &bl);
	if (e) {
		Gunc_nerr(e, "failed to init byte looker");
		e = __LINE__;
		goto fin;
	}

	e = Zoop_decompress(&bys, &bw, &bl);
	if (e) {
		Gunc_nerr(e, "failed to decompress data");
		e = __LINE__;
		goto fin;
	}

	Whine_Canvas_filicide(&canvas);

	//
	



	fin:

	Whine_Easel_filicide(&easel);
	Whine_Canvas_filicide(&canvas);
	Whine_Canvas_filicide(&decompressedCanvas);

	if (f != NULL) {
		e = fclose(f);
		if (e) {
			Gunc_nerr(e, "failed to close file!");
			e = -1;
		}
	}

	return e;
}




/*


int Pork_createPixie(const char *path, struct Whine_Pixie *destination) {

	int e = 0;

	FILE *f = NULL;

	struct Whine_Easel easel = {0};

	struct Gunc_iByteStream bys = {0};


	e = Pork_unwrapZlib(&bys, &easel);
	if (e) {
		Gunc_nerr(e, "failed to unwrap zlib data");
		e = __LINE__;
		goto fin;
	}



	e = Pork_decompress(&easel);
	if (e) {
		Gunc_nerr(e, "failed to decompress");
		e = __LINE__;
		goto fin;
	}


	e = Pork_defilter(&easel);
	if (e) {
		Gunc_nerr(e, "failed to defilter");
		e = __LINE__;
		goto fin;
	}


	e = Whine_Pixie_init(destination, easel);
	if (e) {
		Gunc_nerr(e, "failed to init pixie");
		e = __LINE__;
		goto fin;
	}
	easel = (struct Whine_Easel){0};



	fin:

	Whine_Easel_filicide(&easel);

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

static inline int Pork_decompress(struct Whine_Easel *easel) {

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

	if (easel->hnEaselData == NULL) {
		Gunc_err("null easel data");
		e = __LINE__;
		goto fin;
	}
	struct Gunc_ByteSeq64 seq = {
		.data = easel->hnEaselData,
		.length = easel->easelDataLength
	};
	e = Gunc_ByteSeq64_as_iByteStream(&seq, &bys);
	if (e) {
		Gunc_nerr(e, "failed to init byte stream from seq64");
		e = __LINE__;
		goto fin;
	}


	e = Gunc_ByteBalloon_trim(&bb);
	if (e) {
		Gunc_nerr(e, "failed to trim byteBalloon");
		e = __LINE__;
		goto fin;
	}

	free(easel->hnEaselData);
	Whine_Easel_setData(easel, bb.hData, Whine_Easel_FILTERED);
	easel->easelDataLength = bb.length;
	bb.hData = NULL;



	fin:

	if (bb.hData != NULL) {
		free(bb.hData);
		bb.hData = NULL;
	}

	return e;
}

static inline int Pork_defilter(struct Whine_Easel *destination) {

	int e = 0;

	if (destination == NULL || destination->hnEaselData == NULL) {
		Gunc_nerr(e, "null input");
		return __LINE__;
	}

	struct Whine_Easel easel = {
		.header = destination->header
	};

	struct Gunc_iByteStream bys = {0};
	struct Gunc_ByteSeq64 seq = {
		.data = destination->hnEaselData,
		.length = destination->easelDataLength
	};
	e = Gunc_ByteSeq64_as_iByteStream(&seq, &bys);
	if (e) {
		Gunc_nerr(e, "failed to init byte stream from seq64");
		e = __LINE__;
		goto fin;
	}

	e = Whine_nofilter(&easel, &bys);
	if (e) {
		Gunc_nerr(e, "failed to undo filters");
		e = __LINE__;
		goto fin;
	}

	free(destination->hnEaselData);
	Whine_Easel_setData(destination, easel.hnEaselData, easel.easelDataStatus);
	destination->easelDataLength = easel.easelDataLength;
	easel = (struct Whine_Easel){0};



	fin:

	Whine_Easel_filicide(&easel);

	return e;
}
*/
