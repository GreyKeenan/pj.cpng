#include "./files.h"

#include "./log.h"

#include <stddef.h>

int Gunc_file_next(FILE *self, uint8_t *nDestination) {
	int byte = fgetc(self);
	if (byte == EOF) {
		return Gunc_iByteStream_END;
	}

	if (nDestination == NULL) {
		return 0;
	}
	*nDestination = byte;
	return 0;
}
int Gunc_file_give(FILE *self, uint8_t byte) {

	int e = fputc(byte, self);
	if (e == EOF) {
		Gunc_nerr(e, "failed to write byte: 0x%x (%d)", byte, byte);
		return 1;
	}

	return 0;
}
