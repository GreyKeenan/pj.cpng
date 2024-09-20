#include <stdio.h>

#include "./dechunk.h"

#include "mike/error.h"
#include "mike/ihdr_impl.h"

#include "mike/decompress/decompress.h"
#include "mike/decompress/state_impl.h"

#include "mike/decompress/iNostalgicWriter_forw.h"
#include "mike/decompress/iNostalgicWriter.h"

#include "utils/iByteTrain.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define CHUNK_NAME_LENGTH 4
#define CHUNK_ISLOWERCASE 0x20
#define CHUNK_CRC_LENGTH 4

#define IHDR_LENGTH 13
#define IHDR_COLORTYPE_INDEXED 3


int mike_dechunk_readInt32(iByteTrain *bt, uint32_t *destination);

static inline int mike_dechunk_readName(iByteTrain *bt, uint8_t* destination);
static inline bool mike_dechunk_compareName(uint8_t *a, uint8_t *b);
int mike_dechunk_eatCRC(iByteTrain *bt); //TODO


int mike_Dechunk_go(iByteTrain *bt, mike_Ihdr *ihdr, Mike_Decompress_iNostalgicWriter *nw) {

	int e = 0;
	uint8_t byte = 0;

	uint32_t chunkLength = 0;
	uint8_t chunkName[CHUNK_NAME_LENGTH + 1] = {0}; // extra space so can print as string

	struct mike_Decompress_State destate = {0};
	destate.nw = nw;

	// IHDR
	// ==================================================

	e = mike_dechunk_readInt32(bt, &chunkLength);
	if (e) goto finalize;
	if (chunkLength != IHDR_LENGTH) {
		e = MIKE_ERROR_PNG_IHDR_LENGTH;
		goto finalize;
	}
	e = mike_dechunk_readName(bt, chunkName);
	if (e) goto finalize;
	if (!mike_dechunk_compareName(chunkName, (uint8_t*)"IHDR")) {
		e = MIKE_ERROR_PNG_IHDR_NOT;
		goto finalize;
	}

	e = mike_dechunk_readInt32(bt, &ihdr->width);
	if (e) goto finalize;
	e = mike_dechunk_readInt32(bt, &ihdr->height);
	if (e) goto finalize;

	if (iByteTrain_chewchew(bt, &ihdr->bitDepth)) {
		e = MIKE_ERROR_EOTL;
		goto finalize;
	}
	if (iByteTrain_chewchew(bt, &ihdr->colorType)) {
		e = MIKE_ERROR_EOTL;
		goto finalize;
	}
	if (iByteTrain_chewchew(bt, &ihdr->compressionMethod)) {
		e = MIKE_ERROR_EOTL;
		goto finalize;
	}
	if (iByteTrain_chewchew(bt, &ihdr->filterMethod)) {
		e = MIKE_ERROR_EOTL;
		goto finalize;
	}
	if (iByteTrain_chewchew(bt, &ihdr->interlaceMethod)) {
		e = MIKE_ERROR_EOTL;
		goto finalize;
	}
	switch (ihdr->colorType) {
		case 0:
			switch (ihdr->bitDepth) {
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
			switch (ihdr->bitDepth) {
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
			switch (ihdr->bitDepth) {
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
	if (ihdr->compressionMethod != 0) {
		e = MIKE_ERROR_PNG_IHDR_COMPRESSIONMETHOD;
		goto finalize;
	}
	if (ihdr->filterMethod != 0) {
		e = MIKE_ERROR_PNG_IHDR_FILTERMETHOD;
		goto finalize;
	}
	if (ihdr->interlaceMethod > 1) {
		e = MIKE_ERROR_PNG_IHDR_INTERLACEMETHOD;
		goto finalize;
	}

	e = mike_dechunk_eatCRC(bt);
	if (e) goto finalize;

	// following chunks
	// ==================================================

	uint8_t flags = 0;
	#define HAVEIDAT 0x01
	#define HAVEPLTE 0x02
	#define PREVIOUSWASIDAT 0x04
	#define DEFLATEOVER 0x08
	while (1) {
		e = mike_dechunk_readInt32(bt, &chunkLength);
		if (e) goto finalize;

		e = mike_dechunk_readName(bt, chunkName);
		if (e) goto finalize;

		if (chunkName[0] & CHUNK_ISLOWERCASE) {
			flags = flags & ~PREVIOUSWASIDAT;

			for (uint32_t i = 0; i < chunkLength; ++i) {
				if (iByteTrain_chewchew(bt, NULL)) {
					e = MIKE_ERROR_EOTL;
					goto finalize;
				}
			}
			e = mike_dechunk_eatCRC(bt);
			if (e) goto finalize;
			continue;
		}

		if (mike_dechunk_compareName(chunkName, (uint8_t*)"IDAT")) {
			if ((flags & HAVEIDAT) && !(flags & PREVIOUSWASIDAT)) {
				e = MIKE_ERROR_PNG_IHDR_NONSEQUENTIAL;
				goto finalize;
			}
			if (ihdr->colorType == IHDR_COLORTYPE_INDEXED && !(flags & HAVEPLTE)) {
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
				e = mike_Decompress_step(&destate, byte);
				switch (e) {
					case 0:
						printf("\n");
						break;
					case mike_Decompress_END:
						printf("\t!\n");
						flags = flags | DEFLATEOVER;
						break;
					default: goto finalize;
				}
			}
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			e = mike_dechunk_eatCRC(bt);
			if (e) goto finalize;

			continue;
		}

		flags = flags & ~PREVIOUSWASIDAT;

		if (mike_dechunk_compareName(chunkName, (uint8_t*)"PLTE")) {
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
			e = mike_dechunk_eatCRC(bt);
			if (e) goto finalize;
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			continue;
		}
		if (mike_dechunk_compareName(chunkName, (uint8_t*)"IEND")) {
			e = mike_dechunk_eatCRC(bt);
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
	return e;
}


int mike_dechunk_readInt32(iByteTrain *bt, uint32_t *destination) {
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
static inline int mike_dechunk_readName(iByteTrain *bt, uint8_t* destination) {
	for (int i = 0; i < CHUNK_NAME_LENGTH; ++i) {
		if (iByteTrain_chewchew(bt, destination + i)) {
			return MIKE_ERROR_EOTL;
		}
	}
	printf("chunk: %s\n", destination);
	return 0;
}
static inline bool mike_dechunk_compareName(uint8_t *a, uint8_t *b) {
	for (int i = 0; i < CHUNK_NAME_LENGTH; ++i) {
		if (a[i] != b[i]) {
			return false;
		}
	}
	return true;
}
int mike_dechunk_eatCRC(iByteTrain *bt) {
	for (int i = 0; i < CHUNK_CRC_LENGTH; ++i) {
		if (iByteTrain_chewchew(bt, NULL)) return MIKE_ERROR_EOTL;
	}
	return 0;
}
