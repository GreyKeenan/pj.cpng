#include <stdio.h>

#include "./mike.h"
#include "./deflate.h"
#include "./deflate_impl.h"

#include "utils/iByteTrain.h"

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

	struct Mike_Deflate_State destate = {0};

	// PNG header
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	for (int i = 0; i < PNG_HEADER_LENGTH; ++i) {
		if (iByteTrain_chewchew(bt, &byte)) {
			return MIKE_ERROR_EOTL;
		}
		
		if (byte != mike_PNG_header[i]) {
			return MIKE_ERROR_PNG_NOT;
		}
	}

	// dechunking
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// IHDR
	// ==================================================

	e = mike_png_readInt32(bt, &chunkLength);
	if (e) return e;
	if (chunkLength != IHDR_LENGTH) {
		return MIKE_ERROR_PNG_IHDR_LENGTH;
	}
	e = mike_chunk_readName(bt, chunkName);
	if (e) return e;
	if (!mike_chunk_compareName(chunkName, (uint8_t*)"IHDR")) {
		return MIKE_ERROR_PNG_IHDR_NOT;
	}

	e = mike_png_readInt32(bt, &ihdr.width);
	if (e) return e;
	e = mike_png_readInt32(bt, &ihdr.height);
	if (e) return e;

	if (iByteTrain_chewchew(bt, &ihdr.bitDepth)) return MIKE_ERROR_EOTL;
	if (iByteTrain_chewchew(bt, &ihdr.colorType)) return MIKE_ERROR_EOTL;
	if (iByteTrain_chewchew(bt, &ihdr.compressionMethod)) return MIKE_ERROR_EOTL;
	if (iByteTrain_chewchew(bt, &ihdr.filterMethod)) return MIKE_ERROR_EOTL;
	if (iByteTrain_chewchew(bt, &ihdr.interlaceMethod)) return MIKE_ERROR_EOTL;
	switch (ihdr.colorType) {
		case 0:
			switch (ihdr.bitDepth) {
				case 1:
				case 2:
				case 4:
				case 8:
				case 16:
					break;
				default: return MIKE_ERROR_PNG_IHDR_BITDEPTH;
			}
			break;
		case 3:
			switch (ihdr.bitDepth) {
				case 1:
				case 2:
				case 4:
				case 8:
					break;
				default: return MIKE_ERROR_PNG_IHDR_BITDEPTH;
			}
			break;
		case 2:
		case 4:
		case 6:
			switch (ihdr.bitDepth) {
				case 8:
				case 16:
					break;
				default: return MIKE_ERROR_PNG_IHDR_BITDEPTH;
			}
			break;
		default: return MIKE_ERROR_PNG_IHDR_COLORTYPE; //invalid
	}
	if (ihdr.compressionMethod != 0) {
		return MIKE_ERROR_PNG_IHDR_COMPRESSIONMETHOD;
	}
	if (ihdr.filterMethod != 0) {
		return MIKE_ERROR_PNG_IHDR_FILTERMETHOD;
	}
	if (ihdr.interlaceMethod > 1) {
		return MIKE_ERROR_PNG_IHDR_INTERLACEMETHOD;
	}

	e = mike_chunk_eatCRC(bt);
	if (e) return e;

	// following chunks
	// ==================================================
	uint8_t flags = 0;
	#define HAVEIDAT 0x01
	#define HAVEPLTE 0x02
	#define PREVIOUSWASIDAT 0x04
	#define DEFLATEOVER 0x08
	while (1) {
		e = mike_png_readInt32(bt, &chunkLength);
		if (e) return e;

		e = mike_chunk_readName(bt, chunkName);
		if (e) return e;

		if (chunkName[0] & CHUNK_ISLOWERCASE) {
			flags = flags & ~PREVIOUSWASIDAT;

			for (uint32_t i = 0; i < chunkLength; ++i) {
				if (iByteTrain_chewchew(bt, NULL)) return MIKE_ERROR_EOTL;
			}
			e = mike_chunk_eatCRC(bt);
			if (e) return e;
			continue;
		}

		if (mike_chunk_compareName(chunkName, (uint8_t*)"IDAT")) {
			if ((flags & HAVEIDAT) && !(flags & PREVIOUSWASIDAT)) {
				return MIKE_ERROR_PNG_IHDR_NONSEQUENTIAL;
			}
			if (ihdr.colorType == IHDR_COLORTYPE_INDEXED && !(flags & HAVEPLTE)) {
				return MIKE_ERROR_PNG_PLTE_MISSING;
			}
			if (flags & DEFLATEOVER) {
				return MIKE_ERROR_PNG_IHDR_EXTRA;
			}
			flags = flags | HAVEIDAT;
			flags = flags | PREVIOUSWASIDAT;

			// DEFLATE
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			for (uint32_t i = 0; i < chunkLength; ++i) {
				if (iByteTrain_chewchew(bt, &byte)) return MIKE_ERROR_EOTL;
				printf("%x", byte);
				e = Mike_Deflate_step(&destate, byte);
				switch (e) {
					case 0:
						printf("\n");
						break;
					case MIKE_DEFLATE_END:
						printf("\t!\n");
						flags = flags | DEFLATEOVER;
						break;
					default: return e;
				}
			}
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			e = mike_chunk_eatCRC(bt);
			if (e) return e;

			continue;
		}

		flags = flags & ~PREVIOUSWASIDAT;

		if (mike_chunk_compareName(chunkName, (uint8_t*)"PLTE")) {
			if (HAVEIDAT) {
				return MIKE_ERROR_PNG_PLTE_ORDER;
			}
			// ...

			//TEMP
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			for (uint32_t i = 0; i < chunkLength; ++i) {
				if (iByteTrain_chewchew(bt, NULL)) return MIKE_ERROR_EOTL;
			}
			e = mike_chunk_eatCRC(bt);
			if (e) return e;
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			continue;
		}
		if (mike_chunk_compareName(chunkName, (uint8_t*)"IEND")) {
			e = mike_chunk_eatCRC(bt);
			if (e) return e;

			break;
		}

		return MIKE_ERROR_PNG_CHUNK_UNKNOWN_CRITICAL;
	}

	if (!(flags & HAVEIDAT)) {
		return MIKE_ERROR_PNG_IDAT_MISSING;
	}
	if (!(flags & DEFLATEOVER)) {
		return MIKE_ERROR_PNG_DEFLATE_INCOMPLETE;
	}

	return 0;
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
