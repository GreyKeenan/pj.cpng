#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
//#include <stdbool.h>
#include <limits.h>

#include "utils/iByteSeeker.h"
#include "utils/iByteSeeker_forw.h"

#include "mike/mike.h"

int FILE_step(void *vself, uint8_t *nDestination) {
	int byte = fgetc(vself);
	if (byte == EOF) {
		return iByteSeeker_ERROR;
	}

	if (nDestination == NULL) {
		return 0;
	}
	*nDestination = byte;

	return 0;
}
int FILE_at(const void *vself, iByteSeeker_position *nDestination) {
	FILE *self = (void *)vself;
	long fposition = ftell(self);
	if (fposition == -1) {
		return iByteSeeker_ERROR;
	}
	return fposition;
}
int FILE_go(void *vself, iByteSeeker_position to) {
	if (to > LONG_MAX) {
		return iByteSeeker_ERROR;
	}
	if (fseek(vself, to, SEEK_SET)) {
		return iByteSeeker_ERROR;
	}
	return 0;
}

FILE * FILE_as_iByteSeeker(const char *path, iByteSeeker *nDestination) {
	
	FILE *f = fopen(path, "rb");
	if (f == NULL) {
		return NULL;
	}
	
	if (nDestination != NULL) {
		*nDestination = (iByteSeeker) {
			.vself = f,
			.step = &FILE_step,
			.at = &FILE_at,
			.go = &FILE_go
		};
	}

	return f;
}

int main(int argc, const char **argv) {

	iByteSeeker bs;
	FILE *f = FILE_as_iByteSeeker("pngs/PNG_transparency_demonstration.png", &bs);
	if (f == NULL) {
		printf("cant open file\n");
		return 1;
	}
	
	// ...

	int e = Mike_decode(&bs, NULL);
	printf("e: %d\n", e);

	fclose(f);

	return 0;
}
