#include <stdio.h>

#include "./mike.h"

#include "./decompress/decompress.h"
#include "./decompress/state_impl.h"

#include "./decompress/iNostalgicWriter_forw.h"
#include "./decompress/iNostalgicWriter.h"

#include "utils/iByteTrain.h"

#include <stdlib.h>
#include <stdbool.h>

enum Mike_Error {
	MIKE_ERROR = 1
	, MIKE_ERROR_EOTL

	, MIKE_ERROR_PNG_NOT
	, MIKE_ERROR_PNG_INT32

	, MIKE_ERROR_PNG_IHDR_NOT
	, MIKE_ERROR_PNG_IHDR_LENGTH
	, MIKE_ERROR_PNG_IHDR_COLORTYPE
	, MIKE_ERROR_PNG_IHDR_BITDEPTH
	, MIKE_ERROR_PNG_IHDR_COMPRESSIONMETHOD
	, MIKE_ERROR_PNG_IHDR_FILTERMETHOD
	, MIKE_ERROR_PNG_IHDR_INTERLACEMETHOD
	, MIKE_ERROR_PNG_IHDR_NONSEQUENTIAL
	, MIKE_ERROR_PNG_IHDR_EXTRA

	, MIKE_ERROR_PNG_PLTE_ORDER
	, MIKE_ERROR_PNG_PLTE_MISSING

	, MIKE_ERROR_PNG_IDAT_MISSING
	, MIKE_ERROR_PNG_DEFLATE_INCOMPLETE

	, MIKE_ERROR_PNG_CHUNK_UNKNOWN_CRITICAL
};

struct mike_IHDR {
	uint32_t width;
	uint32_t height;
	uint8_t bitDepth;
	uint8_t colorType;
	uint8_t compressionMethod;
	uint8_t filterMethod;
	uint8_t interlaceMethod;
};

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

#define CHUNK_NAME_LENGTH 4
#define CHUNK_ISLOWERCASE 0x20
#define CHUNK_CRC_LENGTH 4

#define IHDR_LENGTH 13
#define IHDR_COLORTYPE_INDEXED 3

int mike_png_readInt32(iByteTrain *bt, uint32_t *destination);

static inline int mike_chunk_readName(iByteTrain *bt, uint8_t* destination);
static inline bool mike_chunk_compareName(uint8_t *a, uint8_t *b);
int mike_chunk_eatCRC(iByteTrain *bt); //TODO


int Mike_decode(iByteTrain *bt) {

	int e = 0;
	uint8_t byte = 0;

	uint32_t chunkLength = 0;
	uint8_t chunkName[CHUNK_NAME_LENGTH + 1] = {0}; // extra space so can print as string

	struct mike_IHDR ihdr = {0};

	struct Mike_Decompress_State destate = {0};
	struct mike_Writer writer = mike_Writer_create(1024);
	struct Mike_Decompress_iNostalgicWriter nw = mike_Writer_as_iNostalgicWriter(&writer);
	destate.nw = &nw;

	// PNG header
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	for (int i = 0; i < PNG_HEADER_LENGTH; ++i) {
		if (iByteTrain_chewchew(bt, &byte)) {
			e = MIKE_ERROR_EOTL;
			goto finalize;
		}
		
		if (byte != mike_PNG_header[i]) {
			e = MIKE_ERROR_PNG_NOT;
			goto finalize;
		}
	}

	// dechunking
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// IHDR
	// ==================================================

	e = mike_png_readInt32(bt, &chunkLength);
	if (e) goto finalize;
	if (chunkLength != IHDR_LENGTH) {
		e = MIKE_ERROR_PNG_IHDR_LENGTH;
		goto finalize;
	}
	e = mike_chunk_readName(bt, chunkName);
	if (e) goto finalize;
	if (!mike_chunk_compareName(chunkName, (uint8_t*)"IHDR")) {
		e = MIKE_ERROR_PNG_IHDR_NOT;
		goto finalize;
	}

	e = mike_png_readInt32(bt, &ihdr.width);
	if (e) goto finalize;
	e = mike_png_readInt32(bt, &ihdr.height);
	if (e) goto finalize;

	if (iByteTrain_chewchew(bt, &ihdr.bitDepth)) {
		e = MIKE_ERROR_EOTL;
		goto finalize;
	}
	if (iByteTrain_chewchew(bt, &ihdr.colorType)) {
		e = MIKE_ERROR_EOTL;
		goto finalize;
	}
	if (iByteTrain_chewchew(bt, &ihdr.compressionMethod)) {
		e = MIKE_ERROR_EOTL;
		goto finalize;
	}
	if (iByteTrain_chewchew(bt, &ihdr.filterMethod)) {
		e = MIKE_ERROR_EOTL;
		goto finalize;
	}
	if (iByteTrain_chewchew(bt, &ihdr.interlaceMethod)) {
		e = MIKE_ERROR_EOTL;
		goto finalize;
	}
	switch (ihdr.colorType) {
		case 0:
			switch (ihdr.bitDepth) {
				case 1:
				case 2:
				case 4:
				case 8:
				case 16:
					break;
				default: 
					e = MIKE_ERROR_PNG_IHDR_BITDEPTH;
					goto finalize;
			}
			break;
		case 3:
			switch (ihdr.bitDepth) {
				case 1:
				case 2:
				case 4:
				case 8:
					break;
				default:
					e = MIKE_ERROR_PNG_IHDR_BITDEPTH;
					goto finalize;
			}
			break;
		case 2:
		case 4:
		case 6:
			switch (ihdr.bitDepth) {
				case 8:
				case 16:
					break;
				default:
					e = MIKE_ERROR_PNG_IHDR_BITDEPTH;
					goto finalize;
			}
			break;
		default:
			e = MIKE_ERROR_PNG_IHDR_COLORTYPE; //invalid
			goto finalize;
	}
	if (ihdr.compressionMethod != 0) {
		e = MIKE_ERROR_PNG_IHDR_COMPRESSIONMETHOD;
		goto finalize;
	}
	if (ihdr.filterMethod != 0) {
		e = MIKE_ERROR_PNG_IHDR_FILTERMETHOD;
		goto finalize;
	}
	if (ihdr.interlaceMethod > 1) {
		e = MIKE_ERROR_PNG_IHDR_INTERLACEMETHOD;
		goto finalize;
	}

	e = mike_chunk_eatCRC(bt);
	if (e) goto finalize;

	// following chunks
	// ==================================================

	uint8_t flags = 0;
	#define HAVEIDAT 0x01
	#define HAVEPLTE 0x02
	#define PREVIOUSWASIDAT 0x04
	#define DEFLATEOVER 0x08
	while (1) {
		e = mike_png_readInt32(bt, &chunkLength);
		if (e) goto finalize;

		e = mike_chunk_readName(bt, chunkName);
		if (e) goto finalize;

		if (chunkName[0] & CHUNK_ISLOWERCASE) {
			flags = flags & ~PREVIOUSWASIDAT;

			for (uint32_t i = 0; i < chunkLength; ++i) {
				if (iByteTrain_chewchew(bt, NULL)) {
					e = MIKE_ERROR_EOTL;
					goto finalize;
				}
			}
			e = mike_chunk_eatCRC(bt);
			if (e) goto finalize;
			continue;
		}

		if (mike_chunk_compareName(chunkName, (uint8_t*)"IDAT")) {
			if ((flags & HAVEIDAT) && !(flags & PREVIOUSWASIDAT)) {
				e = MIKE_ERROR_PNG_IHDR_NONSEQUENTIAL;
				goto finalize;
			}
			if (ihdr.colorType == IHDR_COLORTYPE_INDEXED && !(flags & HAVEPLTE)) {
				e = MIKE_ERROR_PNG_PLTE_MISSING;
				goto finalize;
			}
			if (flags & DEFLATEOVER) {
				e = MIKE_ERROR_PNG_IHDR_EXTRA;
				goto finalize;
			}
			flags = flags | HAVEIDAT;
			flags = flags | PREVIOUSWASIDAT;

			// DEFLATE
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			for (uint32_t i = 0; i < chunkLength; ++i) {
				if (iByteTrain_chewchew(bt, &byte)) {
					e = MIKE_ERROR_EOTL;
					goto finalize;
				}
				printf("%x", byte);
				e = Mike_Decompress_step(&destate, byte);
				switch (e) {
					case 0:
						printf("\n");
						break;
					case Mike_Decompress_END:
						printf("\t!\n");
						flags = flags | DEFLATEOVER;
						break;
					default: goto finalize;
				}
			}
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			e = mike_chunk_eatCRC(bt);
			if (e) goto finalize;

			continue;
		}

		flags = flags & ~PREVIOUSWASIDAT;

		if (mike_chunk_compareName(chunkName, (uint8_t*)"PLTE")) {
			if (HAVEIDAT) {
				e = MIKE_ERROR_PNG_PLTE_ORDER;
				goto finalize;
			}
			// ...

			//TEMP
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			for (uint32_t i = 0; i < chunkLength; ++i) {
				if (iByteTrain_chewchew(bt, NULL)){ 
					e = MIKE_ERROR_EOTL;
					goto finalize;
				}
			}
			e = mike_chunk_eatCRC(bt);
			if (e) goto finalize;
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			continue;
		}
		if (mike_chunk_compareName(chunkName, (uint8_t*)"IEND")) {
			e = mike_chunk_eatCRC(bt);
			if (e) goto finalize;

			break;
		}

		e = MIKE_ERROR_PNG_CHUNK_UNKNOWN_CRITICAL;
		goto finalize;
	}

	if (!(flags & HAVEIDAT)) {
		e = MIKE_ERROR_PNG_IDAT_MISSING;
		goto finalize;
	}
	if (!(flags & DEFLATEOVER)) {
		e = MIKE_ERROR_PNG_DEFLATE_INCOMPLETE;
		goto finalize;
	}

	finalize:
	mike_Writer_destroy(&writer);
	return e;
}

int mike_png_readInt32(iByteTrain *bt, uint32_t *destination) {
	/*
		reads a big-endian uint32 as outlined in PNG spec
		guarantees that the value is only an int31 as per spec
	*/
	uint8_t byte = 0;
	uint32_t n = 0;

	for (int i = 0; i < 4; ++i) {
		if (iByteTrain_chewchew(bt, &byte)) {
			return MIKE_ERROR_EOTL;
		}
		n = (n << 8) | byte;
	}

	if (n & 0x80000000) {
		return MIKE_ERROR_PNG_INT32;
	}

	*destination = n;
	return 0;
}
static inline int mike_chunk_readName(iByteTrain *bt, uint8_t* destination) {
	for (int i = 0; i < CHUNK_NAME_LENGTH; ++i) {
		if (iByteTrain_chewchew(bt, destination + i)) {
			return MIKE_ERROR_EOTL;
		}
	}
	printf("chunk: %s\n", destination);
	return 0;
}
static inline bool mike_chunk_compareName(uint8_t *a, uint8_t *b) {
	for (int i = 0; i < CHUNK_NAME_LENGTH; ++i) {
		if (a[i] != b[i]) {
			return false;
		}
	}
	return true;
}
int mike_chunk_eatCRC(iByteTrain *bt) {
	for (int i = 0; i < CHUNK_CRC_LENGTH; ++i) {
		if (iByteTrain_chewchew(bt, NULL)) return MIKE_ERROR_EOTL;
	}
	return 0;
}
