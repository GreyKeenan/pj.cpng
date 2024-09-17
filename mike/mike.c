#include <stdio.h>

#include "./mike.h"

#include "utils/endian.h"

#include "utils/iByteSeeker.h"
#include "utils/byteJumper.h"
#include "utils/byteJumper_impl.h"

#include <stdbool.h>

#define MIKE_ERROR 1
#define MIKE_ERROR_EOF 2
#define MIKE_ERROR_PNG_NOT 3
#define MIKE_ERROR_PNG_IHDR_NOT 4
#define MIKE_ERROR_PNG_IHDR_LENGTH 5

#define PNG_HEADER_LENGTH 8
const uint8_t _Mike_PNG_header[PNG_HEADER_LENGTH] = {0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A};

#define CHUNK_NAME_LENGTH 4
#define CRC_LENGTH 4

#define IHDR_LENGTH 13


int Mike_PNG_dechunk(iByteSeeker *bs, ByteJumper *bj);


int Mike_decode(iByteSeeker *bs, void *destination) {

	// interpret PNG chunks -> iByteSeeker & size information
	// zlib/DEFLATE iBitByteJumper -> array_of_bytes -> iBitByteTrain
	// PNG decompress iBitByteTrain -> referenceImage

	ByteJumper bj = {0};

	int e = Mike_PNG_dechunk(bs, &bj);
	if (e) return e;

	return e;
}

int Mike_readUint32(iByteSeeker *bs, uint32_t *destination) {

	uint8_t bytes[4] = {0};
	for (int i = 0; i < 4; ++i) {
		if (iByteSeeker_step(bs, bytes + i)) {
			return MIKE_ERROR_EOF;
		}
	}

	if (ENDIAN_ISLITTLE) {
		Endian_flip(bytes, 4);
	}

	*destination = *(uint32_t*)bytes;
	
	return 0;
}
int Mike_PNG_compareChunkString(iByteSeeker *bs, const char *name, bool *destination) {
	uint8_t byte = 0;

	for (int i = 0; i < CHUNK_NAME_LENGTH; ++i) {
		if (iByteSeeker_step(bs, &byte)) {
			return MIKE_ERROR_EOF;
		}
		if (byte != name[i]) {
			*destination = false;
			return 0;
		}
	}
	*destination = true;
	return 0;
}
int Mike_PNG_eatCRC(iByteSeeker *bs) { //TODO TEMP
	for (int i = 0; i < CRC_LENGTH; ++i) {
		if (iByteSeeker_step(bs, NULL)) {
			return MIKE_ERROR_EOF;
		}
	}
	return 0;
}

int Mike_PNG_dechunk(iByteSeeker *bs, ByteJumper *bj) {
	
	int e = 0;
	uint8_t byte = 0;

	//PNG header
	for (int i = 0; i < PNG_HEADER_LENGTH; ++i) {
		if (iByteSeeker_step(bs, &byte)) {
			return MIKE_ERROR_EOF;
		}
		
		if (byte != _Mike_PNG_header[i]) {
			return MIKE_ERROR_PNG_NOT;
		}
	}

	//IHDR
	uint32_t int32_destination = 0;
	e = Mike_readUint32(bs, &int32_destination); //chunk length
	if (e) return e;
	if (int32_destination != IHDR_LENGTH) {
		return MIKE_ERROR_PNG_IHDR_LENGTH;
	}

	bool isChunk = 0;
	e = Mike_PNG_compareChunkString(bs, "IHDR", &isChunk); //chunk name
	if (e) return e;
	if (!isChunk) {
		return MIKE_ERROR_PNG_IHDR_NOT;
	}

	/*
	e = Mike_readUint32(bs, &int32_destination); //width
	if (e) return e;
	// validate width, store it
	e = Mike_readUint32(bs, &int32_destination); //length
	if (e) return e;
	// validate length, store it
	*/
	
	return 0;
}
