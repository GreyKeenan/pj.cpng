#ifndef XYLB_ERROR_H
#define XYLB_ERROR_H

#include <limits.h>

enum Xylb_decompress_Error {
	Xylb_decompress_END = 1
	, Xylb_decompress_IMPOSSIBLE

	, Xylb_decompress_ERROR

	, Xylb_decompress_ERROR_HEADERING_BYTESREAD
	, Xylb_decompress_ERROR_COMPRESSIONMETHOD
	, Xylb_decompress_ERROR_WINDOWSIZE
	, Xylb_decompress_ERROR_HASDICT
	, Xylb_decompress_ERROR_SHIRT

	, Xylb_decompress_ERROR_ADLERING_BYTESREAD
	, Xylb_decompress_ERROR_ADLER32

};

#endif
