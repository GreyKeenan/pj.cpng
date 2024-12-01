
#include "./files.h"

#include "./iByteStream.h"
#include "./log.h"
#include <stddef.h>

int Gunc_file_next(void *vself, uint8_t *nDestination) {
	int byte = fgetc(vself);
	if (byte == EOF) {
		return Gunc_iByteStream_END;
	}

	if (nDestination == NULL) {
		return 0;
	}
	*nDestination = byte;
	return 0;
}

int Gunc_file_as_iByteStream(FILE **f, struct Gunc_iByteStream *bs, const char *path) {
	if (f == NULL || bs == NULL || path == NULL) {
		Gunc_err("NULL pointer arguments.");
		return 1;
	}

	*f = fopen(path, "rb");
	if (*f == NULL) {
		Gunc_err("failed to open file: %s", path);
		return 2;
	}

	int e = Gunc_iByteStream_init(bs, *f, &Gunc_file_next);
	if (e) {
		Gunc_nerr(e, "failed to initialize iByteStream.");
		return 3;
	}

	return 0;
}
