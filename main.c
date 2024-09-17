#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
//#include <stdbool.h>
#include <limits.h>

//#include "utils/iByteSeeker.h"
//#include "utils/iByteSeeker_forw.h"
#include "utils/iByteTrain_forw.h"
#include "utils/iByteTrain_impl.h"
#include "utils/iByteTrain.h"

#include "mike/mike.h"

int FILE_chewchew(void *vself, uint8_t *nDestination) {
	int byte = fgetc(vself);
	if (byte == EOF) {
		return iByteTrain_ENDOFTHELINE;
	}

	if (nDestination == NULL) {
		return 0;
	}
	*nDestination = byte;

	return 0;
}

FILE *FILE_as_iByteTrain(const char *path, iByteTrain *destination) {
	
	FILE *f = fopen(path, "rb");
	if (f == NULL) {
		return NULL;
	}

	*destination = (iByteTrain) {
		.vself = f,
		.chewchew = &FILE_chewchew
	};
	return f;

}

int main(int argc, const char **argv) {

	iByteTrain bt;
	FILE *f = FILE_as_iByteTrain("pngs/PNG_transparency_demonstration.png", &bt);
	if (f == NULL) {
		printf("cant open file\n");
		return 1;
	}
	
	// ...

	int e = Mike_decode(&bt);
	printf("\nfinal error status: %d\n", e);

	fclose(f);

	return 0;
}
