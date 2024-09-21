#ifndef MIKE_DECHUNK_ERROR_H
#define MIKE_DECHUNK_ERROR_H

#include "mike/error.h"

enum Mike_Dechunk_Error {
	Mike_Dechunk_Error_TYPE = Mike_ERRORTYPE_DECHUNK

	, Mike_Dechunk_ERROR
	, Mike_Dechunk_ERROR_EOTL
	, Mike_Dechunk_ERROR_INT32

	, Mike_Dechunk_ERROR_IHDR_NOT
	, Mike_Dechunk_ERROR_IHDR_LENGTH
	, Mike_Dechunk_ERROR_IHDR_COLORTYPE
	, Mike_Dechunk_ERROR_IHDR_BITDEPTH
	, Mike_Dechunk_ERROR_IHDR_COMPRESSIONMETHOD
	, Mike_Dechunk_ERROR_IHDR_FILTERMETHOD
	, Mike_Dechunk_ERROR_IHDR_INTERLACEMETHOD
	, Mike_Dechunk_ERROR_IHDR_NONSEQUENTIAL
	, Mike_Dechunk_ERROR_IHDR_EXTRA

	, Mike_Dechunk_ERROR_PLTE_ORDER
	, Mike_Dechunk_ERROR_PLTE_MISSING

	, Mike_Dechunk_ERROR_IDAT_MISSING
	, Mike_Dechunk_ERROR_DEFLATE_INCOMPLETE

	, Mike_Dechunk_ERROR_CHUNK_UNKNOWN_CRITICAL


	, Mike_Dechunk_Error_MAX
};

#endif
