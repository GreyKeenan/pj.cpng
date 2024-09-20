#include <stdio.h>

#include "./mike.h"
#include "./error.h"
#include "./ihdr_impl.h"

#include "./dechunk/dechunk.h"

#include "./decompress/iNostalgicWriter_forw.h"
#include "./decompress/iNostalgicWriter.h"

#include "utils/iByteTrain.h"

#include <stdlib.h>


// mike_Writer
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

typedef struct mike_Writer mike_Writer;
struct mike_Writer {
	uint8_t *nData;
	uint16_t length;
	uint16_t cap;
	uint16_t step;
};

mike_Writer mike_Writer_create(uint16_t step) {
	if (step == 0) step++;

	return (mike_Writer) {
		.nData = NULL,
		.step = step
	};
}
void mike_Writer_destroy(mike_Writer *self) {
	if (self->nData != NULL) {
		free(self->nData);
	}
}

int mike_Writer_write(void *vself, uint8_t byte) {
	mike_Writer *self = vself;
	void *tempPtr = NULL;

	if (self->length >= self->cap) {

		tempPtr = realloc(self->nData, self->cap + self->step);
		if (tempPtr == NULL) {
			return Mike_Decompress_iNostalgicWriter_TOOFAR;
		}

		self->cap += self->step;
		self->nData = tempPtr;
	}

	self->nData[self->length] = byte;
	self->length++;
	return 0;
}
int mike_Writer_nostalgize(const void *vself, uint8_t *destination, uint32_t distanceBack) {
	const mike_Writer *self = vself;

	if (distanceBack > self->length) {
		return Mike_Decompress_iNostalgicWriter_TOOFAR;
	}

	*destination = self->nData[self->length - distanceBack];
	return 0;
}
Mike_Decompress_iNostalgicWriter mike_Writer_as_iNostalgicWriter(mike_Writer *self) {
	return (Mike_Decompress_iNostalgicWriter) {
		.vself = self,
		.write = &mike_Writer_write,
		.nostalgize = &mike_Writer_nostalgize
	};
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#define PNG_HEADER_LENGTH 8
const uint8_t mike_PNG_header[PNG_HEADER_LENGTH] = {0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A};

int Mike_decode(iByteTrain *bt) {

	int e = 0;
	uint8_t byte = 0;

	// PNG header
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	for (int i = 0; i < PNG_HEADER_LENGTH; ++i) {
		if (iByteTrain_chewchew(bt, &byte)) {
			e = Mike_ERROR_EOTL;
			goto finalize;
		}
		
		if (byte != mike_PNG_header[i]) {
			e = Mike_ERROR_PNG_NOT;
			goto finalize;
		}
	}

	// dechunking & decompressing
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	struct mike_Ihdr ihdr = {0};

	struct mike_Writer writer = mike_Writer_create(1024);
	struct Mike_Decompress_iNostalgicWriter nw = mike_Writer_as_iNostalgicWriter(&writer);

	e = mike_Dechunk_go(bt, &ihdr, &nw);
	if (e) return e;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if (ihdr.interlaceMethod == 1) {
		printf("TODO ERR: ADAM7 INTERLACED\n");
		e = Mike_ERROR;
		goto finalize;
	}
	if (ihdr.colorType == 3) {
		printf("TODO ERR: INDEXED PALETTE COLORTYPE\n");
		e = Mike_ERROR;
		goto finalize;
	}


	finalize:
	mike_Writer_destroy(&writer);
	return e;
}
