#ifndef MIKE_DECOMPRESS_H
#define MIKE_DECOMPRESS_H

#include "./state_forw.h"

#include <stdint.h>

enum mike_Decompress_Error {
	mike_Decompress_END = -101
		// END is returned when the final byte is processed AND when subsequent bytes are fed in.

	, mike_Decompress_ERROR = -100
	, mike_Decompress_ERROR_STATE_UNKNOWN

	, mike_Decompress_ERROR_WRITER_WRITE
	, mike_Decompress_ERROR_WRITER_NOSTALGIZE

	, mike_Decompress_ERROR_ZLIBHEADER_OVERREAD
	, mike_Decompress_ERROR_ZLIBHEADER_COMPRESSIONMETHOD
	, mike_Decompress_ERROR_ZLIBHEADER_WINDOWSIZE
	, mike_Decompress_ERROR_ZLIBHEADER_VALIDATION
	, mike_Decompress_ERROR_ZLIBHEADER_HASDICT

	, mike_Decompress_ERROR_BLOCKHEADER_OVERREAD
	, mike_Decompress_ERROR_BLOCKHEADER_COMPRESSIONTYPE

	, mike_Decompress_ERROR_UNCOMPRESSED_LENGTH_OVERREAD
	, mike_Decompress_ERROR_UNCOMPRESSED_NLEN
	, mike_Decompress_ERROR_UNCOMPRESSED_OVERREAD

	, mike_Decompress_ERROR_ADLER32_OVERREAD
	, mike_Decompress_ERROR_ADLER32_FAILED
};

int mike_Decompress_step(mike_Decompress_State *state, uint8_t byte);

#endif
