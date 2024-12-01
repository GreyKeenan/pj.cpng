
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "gunc/log.h"
#include "gunc/iByteStream.h"
#include "gunc/iByteWriter.h"
#include "gunc/files.h"
#include "gunc/byteBalloon.h"

#include "whine/stripng.h"
#include "whine/image.h"

int main(int argc, char **argv) {
	Gunc_say("Starting Program: %s", argv[0]);

	int e = 0;

	struct Gunc_iByteStream bs = {0};
	struct Gunc_iByteWriter bw = {0};

	FILE *f = NULL;
	struct Gunc_ByteBalloon bb = {0};

	struct Whine_Image image = {0};

	if (argc < 2) {
		Gunc_err("missing png file path");
		goto fin;
	}

	e = Gunc_file_as_iByteStream(&f, &bs, argv[1]);
	if (e) {
		Gunc_nerr(e, "Failed to open file: %s", argv[1]);
		goto fin;
	}

	e = Gunc_ByteBalloon_init(&bb, 1024);
	if (e) {
		Gunc_nerr(e, "failed to init byteBalloon %p", &bb);
		goto fin;
	}

	e = Gunc_ByteBalloon_as_iByteWriter(&bb, &bw);
	if (e) {
		Gunc_nerr(e, "failed to create iByteWriter from byteBalloon");
		goto fin;
	}

	e = Whine_stripng(&image, &bs, &bw);
	if (e) {
		Gunc_nerr(e, "failed to extract zlib data from png");
		goto fin;
	}



	fin:

	if (f != NULL) {
		fclose(f);
	}

	Gunc_say("Program endpoint reached.");
	return 0;
}

#include "gunc/log.c"
#include "gunc/files.c"
#include "gunc/byteBalloon.c"

#include "whine/all.c"
