
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "gunc/log.h"
#include "gunc/iByteStream.h"
#include "gunc/iByteWriter.h"
#include "gunc/files.h"
#include "gunc/byteBalloon.h"
#include "gunc/sequence.h"

#include "whine/stripng.h"
#include "whine/image.h"

#include "zoop/decompress.h"

int main(int argc, char **argv) {
	Gunc_title("Starting Program: %s", argv[0]);

	int e = 0;

	struct Gunc_iByteStream bs = {0};
	struct Gunc_iByteWriter bw = {0};

	FILE *f = NULL;
	struct Gunc_ByteBalloon bb_whine = {0};

	struct Whine_Image image = {0};

	struct Gunc_ByteBalloon bb_zoop = {0};
	struct Gunc_Sequence seq = {0};

	if (argc < 2) {
		Gunc_err("missing png file path");
		goto fin;
	}
	Gunc_say("filepath: %s", argv[1]);


	Gunc_title("Whine_stripng Setup");

	e = Gunc_file_as_iByteStream(&f, &bs, argv[1]);
	if (e) {
		Gunc_nerr(e, "Failed to open file: %s", argv[1]);
		goto fin;
	}

	e = Gunc_ByteBalloon_init(&bb_whine, 1024);
	if (e) {
		Gunc_nerr(e, "failed to init byteBalloon %p", &bb_whine);
		goto fin;
	}
	e = Gunc_ByteBalloon_as_iByteWriter(&bb_whine, &bw);
	if (e) {
		Gunc_nerr(e, "failed to create iByteWriter from byteBalloon");
		goto fin;
	}


	Gunc_title("Stripping PNG");

	e = Whine_stripng(&image, &bs, &bw);
	if (e) {
		Gunc_nerr(e, "failed to extract zlib data from png");
		goto fin;
	}


	Gunc_title("Whine Closeout");

	e = Gunc_ByteBalloon_trim(&bb_whine);
	if (e) {
		Gunc_nerr(e, "failed to trim byteBalloon.");
		goto fin;
	}

	e = fclose(f);
	if (e) {
		Gunc_nwarn(e, "failed to close file (ptr: %p, path: %s). Continuing ... ", f, argv[1]);
	} else {
		f = NULL;
	}


	Gunc_title("Zoop setup");

	e = Gunc_ByteBalloon_init(&bb_zoop, 1024);
	if (e) {
		Gunc_nerr(e, "failed to init byteBalloon for zoop.");
		goto fin;
	}
	e = Gunc_ByteBalloon_as_iByteWriter(&bb_zoop, &bw);
	if (e) {
		Gunc_nerr(e, "failed to create iByteWriter from byteBalloon for zoop.");
		goto fin;
	}

	e = Gunc_Sequence_init(&seq, bb_whine.hData, bb_whine.length, sizeof(*bb_whine.hData));
	if (e) {
		Gunc_nerr(e, "failed to init Sequence from bb_whine");
		goto fin;
	}
	e = Gunc_Sequence_as_iByteStream(&seq, &bs);
	if (e) {
		Gunc_nerr(e, "failed to create iByteStream from Sequence");
		goto fin;
	}

	
	Gunc_title("Decompressing Zlib Bytes");

	e = Zoop_decompress(&bs, &bw);
	if (e) {
		Gunc_nerr(e, "failed to zoop the zoopidee zoop");
		goto fin;
	}


	fin:

	if (f != NULL) {
		e = fclose(f);
		if (e) {
			Gunc_nwarn(e, "failed to close file (ptr: %p, path: %s). Program terminating soon anyways.", f, argv[1]);
		}
	}

	if (bb_whine.hData != NULL) {
		free(bb_whine.hData);
	}
	if (bb_zoop.hData != NULL) {
		free(bb_zoop.hData);
	}


	Gunc_title("Program endpoint reached.");
	return 0;
}

#include "gunc/log.c"
#include "gunc/files.c"
#include "gunc/byteBalloon.c"
#include "gunc/sequence.c"

#include "whine/all.c"
#include "zoop/all.c"
