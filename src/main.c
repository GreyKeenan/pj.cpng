
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "./pix_as_imt.h"

#include "gunc/log.h"

#include "whine/pixie.h"

#include "pork/createPixie.h"

#include "sdaubler/display.h"
#include "sdaubler/iImageTrain_impl.h"


int main(int argc, char **argv) {
	Gunc_title("Starting Program: %s", argv[0]);

	int e = 0;


	if (argc < 2) {
		Gunc_err("missing png file path");
		goto fin;
	}
	Gunc_say("filepath: %s", argv[1]);



	Gunc_title("Decompressing Image!");

	struct Whine_Pixie pixie = {0};
	e = Pork_createPixie(argv[1], &pixie);
	if (e) {
		Gunc_nerr(e, "failed to create pixie");
		goto fin;
	}


	Gunc_title("displaying image!");

	struct Sdaubler_iImageTrain imt = {0};
	e = _Pixie_as_iImageTrain(&pixie, &imt);
	if (e) {
		Gunc_nerr(e, "failed to init imt");
		return __LINE__;
	}

	e = Sdaubler_display(&imt);
	if (e) {
		Gunc_nerr(e, "Sdaubler_display failed");
		return __LINE__;
	}



	fin:

	Whine_Image_filicide(&pixie.image);

	return (bool)e;
}
