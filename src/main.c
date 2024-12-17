
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "gunc/log.h"

#include "whine/pixie.h"

#include "pork/createPixie.h"

#include <sys/ioctl.h>
#include <unistd.h>

// #define WIDTH 80
#define PIXEL "  "
#define PXW 2

void _printPixel(uint32_t p);
int _printImage(const struct Gunc_iRuneStream *rs, int32_t w, int32_t h);


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

	struct Gunc_iRuneStream rs = {0};
	e = Whine_Pixie_as_iRuneStream(&pixie, &rs);
	if (e) {
		Gunc_nerr(e, "failed to init rune stream");
		goto fin;
	}

	Gunc_title("Image:");

	e = _printImage(
		&rs,
		pixie.easel.header.width,
		pixie.easel.header.height
	);
	if (e) {
		Gunc_nerr(e, "failed to print image");
		goto fin;
	}



	fin:

	Whine_Easel_filicide(&pixie.easel);
	Whine_Canvas_filicide(&pixie.canvas);

	Gunc_title("Program Endpoint (%d)", (bool)e);

	return (bool)e;
}

int _printImage(const struct Gunc_iRuneStream *rs, int32_t w, int32_t h) {

	int e = 0;

	uint32_t pixel = 0;

	struct winsize win;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);

	Gunc_say("image dimenisons: %dx%d", w, h);

	for (int32_t j = 0; j < h; ++j) {
		for (int32_t i = 0; i < w; ++i) {
			e = Gunc_iRuneStream_next(rs, &pixel);
			if (e) {
				Gunc_nerr(e, "failed to read next pixel (%dx%d) of (%dx%d)",
					i, j,
					w,h
				);
				_printPixel(0xff000000);
				e = 1;
				goto fin;
			}

			if (i <= win.ws_col / PXW - 1) {
				_printPixel(pixel);
			}
		}

		printf("[0m\n");
	}

	fin:
	printf("[0m\n");
	return e;
}

void _printPixel(uint32_t p) {
	uint8_t r = p >> 24;
	uint8_t g = p >> 16;
	uint8_t b = p >> 8;

	printf("[48;2;%d;%d;%dm" PIXEL, r, g, b);
}
