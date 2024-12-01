#ifndef GUNC_FILES_H
#define GUNC_FILES_H

#include <stdint.h>
#include <stdio.h>

struct Gunc_iByteStream;

int Gunc_file_next(void *vself, uint8_t *nDestination);

int Gunc_file_as_iByteStream(FILE **f, struct Gunc_iByteStream *bs, const char *path);
/*
	create an iByteStream from a f stream, gives to $bs
	gives FILE* to $f, it must be closed later.
		as an "rb" type
	returns 0 on success
		1 on null function arguments
		2 on failure to open file
		3 on failure to init iByteStream
*/

#endif
