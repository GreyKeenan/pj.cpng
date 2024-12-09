#ifndef GUNC_FILES_H
#define GUNC_FILES_H

#include "./iByteStream.h"
#include "./iByteWriter.h"

#include <stdint.h>
#include <stdio.h>

int Gunc_file_next(FILE *self, uint8_t *nDestination);
int Gunc_file_give(FILE *self, uint8_t byte);

static inline int Gunc_file_as_iByteStream(FILE *self, struct Gunc_iByteStream *bs) {
	/*
	will try to read from the file.
	'rb' mode is recommended
	*/
	return Gunc_iByteStream_init(bs, self,
		(int(*)(void*, uint8_t*))&Gunc_file_next
	);
}
static inline int Gunc_file_as_iByteWrite(FILE *self, struct Gunc_iByteWriter *bw) {
	/*
	will try to write to the file
	'wb' mode is recommended
	*/
	return Gunc_iByteWriter_init(bw, self,
		(int(*)(void*, uint8_t))&Gunc_file_give
	);
}

#endif
