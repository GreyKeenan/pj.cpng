#ifndef MIKE_ERROR_H
#define MIKE_ERROR_H

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

#endif