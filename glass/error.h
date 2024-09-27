#ifndef Mike_ERROR_H
#define Mike_ERROR_H

enum Mike_Error {
	Mike_ERROR = 1

	, Mike_ERROR_EOTL
	, Mike_ERROR_NOTPNG

};

#define Mike_ERRORTYPE_DECHUNK 0x1000
#define Mike_ERRORTYPE_DECOMPRESS 0x2000
#define Mike_ERRORTYPE_DEFILTER 0x3000
/*
	easy way to ensure errors are unique btwn these sections
	subject to change

*/


#endif
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
#ifndef MIKE_DEFILTER_ERROR_H
#define MIKE_DEFILTER_ERROR_H

#include "mike/error.h"

enum Mike_Defilter_Error {
	Mike_Defilter_Error_TYPE = Mike_ERRORTYPE_DEFILTER

	, Mike_Defilter_ERROR
	, Mike_Defilter_ERROR_EOTL

	, Mike_Defilter_ERROR_MALLOC

	, Mike_Defilter_ERROR_COLORTYPE
	, Mike_Defilter_ERROR_FILTERTYPE

};
#endif
