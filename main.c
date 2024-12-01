
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "gunc/log.h"
#include "gunc/iByteStream.h"
#include "gunc/files.h"

int main(int argc, char **argv) {
	Gunc_say("Starting Program: %s", argv[0]);

	struct Gunc_iByteStream bs = {0};
	FILE *f = NULL;
	int e = 0;

	if (argc < 2) {
		Gunc_err("missing png file path");
		goto fin;
	}

	e = Gunc_file_as_iByteStream(&f, &bs, argv[1]);
	if (e) {
		Gunc_nerr(e, "Failed to open file: %s", argv[1]);
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
