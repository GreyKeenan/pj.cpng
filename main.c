#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "./utils/iByteTrain.h"
#include "./utils/iByteTrain_forw.h"


int _FilePtr_chewchew(void *vself, uint8_t *destination) {

	int byte = fgetc(vself);

	if (byte == EOF) {
		return IBYTETRAIN_ENDOFTHELINE;
	}

	if (destination != NULL) {
		*destination = byte;
	}

	return 0;
}
int _FilePtr_lookoutAhead(const void *vself, uint8_t *destination) {
	FILE *self = (void *)vself;

	int byte = fgetc(self);

	if (byte == EOF) {
		return IBYTETRAIN_ENDOFTHELINE;
	}

	ungetc(byte, self);

	if (destination != NULL) {
		*destination = byte;
	}

	return 0;
}
iByteTrain FilePtr_as_iByteTrain(FILE *self) {
	return (iByteTrain) {
		.vself = self,
		.chewchew = &_FilePtr_chewchew,
		.lookoutAhead = &_FilePtr_lookoutAhead
	};
}

int main(int argc, const char **argv) {

	FILE *f = fopen("pngs/PNG_transparency_demonstration.png", "r");
	if (f == NULL) {
		return 1;
	}
	
	iByteTrain bt = FilePtr_as_iByteTrain(f);

	//feed byte reader to PNG decoding
		// extract the IDAT data from the PNG and put it into some sort of BitByteReader
		// feed bbr to zlib-standin
		// ...
	
	fclose(f);

	return 0;
}
