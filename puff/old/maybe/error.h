#ifndef MIKE_DECOMPRESS_ERROR_H
#define MIKE_DECOMPRESS_ERROR_H

#include "mike/error.h"

enum Mike_Decompress_Error {
	Mike_Decompress_Error_TYPE = Mike_ERRORTYPE_DECOMPRESS
	
	, Mike_Decompress_END
		// END is returned when the final byte is processed AND when subsequent bytes are fed in.

	, Mike_Decompress_ERROR
	, Mike_Decompress_ERROR_STATE_UNKNOWN

	, Mike_Decompress_ERROR_WRITER_WRITE
	, Mike_Decompress_ERROR_WRITER_NOSTALGIZE

	, Mike_Decompress_ERROR_ZLIBHEADER_OVERREAD
	, Mike_Decompress_ERROR_ZLIBHEADER_COMPRESSIONMETHOD
	, Mike_Decompress_ERROR_ZLIBHEADER_WINDOWSIZE
	, Mike_Decompress_ERROR_ZLIBHEADER_VALIDATION
	, Mike_Decompress_ERROR_ZLIBHEADER_HASDICT

	, Mike_Decompress_ERROR_BLOCKHEADER_OVERREAD
	, Mike_Decompress_ERROR_BLOCKHEADER_COMPRESSIONTYPE

	, Mike_Decompress_ERROR_UNCOMPRESSED_LENGTH_OVERREAD
	, Mike_Decompress_ERROR_UNCOMPRESSED_NLEN
	, Mike_Decompress_ERROR_UNCOMPRESSED_OVERREAD

	, Mike_Decompress_ERROR_ADLER32_OVERREAD
	, Mike_Decompress_ERROR_ADLER32_FAILED
};

#endif
