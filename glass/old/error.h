#ifndef GLASS_ERROR_H
#define GLASS_ERROR_H

enum Glass_Error {
	Glass_ERROR = 1

	, Glass_ERROR_EOTL
	, Glass_ERROR_NOTPNG

};

#define Glass_ERRORTYPE_DECHUNK 0x1000
#define Glass_ERRORTYPE_DECOMPRESS 0x2000
#define Glass_ERRORTYPE_DEFILTER 0x3000
/*
	easy way to ensure errors are unique btwn these sections
	subject to change

*/

enum Glass_Dechunk_Error {
	Glass_Dechunk_Error_TYPE = Glass_ERRORTYPE_DECHUNK

	, Glass_Dechunk_ERROR
	, Glass_Dechunk_ERROR_EOTL
	, Glass_Dechunk_ERROR_INT32

	, Glass_Dechunk_ERROR_IHDR_NOT
	, Glass_Dechunk_ERROR_IHDR_LENGTH
	, Glass_Dechunk_ERROR_IHDR_COLORTYPE
	, Glass_Dechunk_ERROR_IHDR_BITDEPTH
	, Glass_Dechunk_ERROR_IHDR_COMPRESSIONMETHOD
	, Glass_Dechunk_ERROR_IHDR_FILTERMETHOD
	, Glass_Dechunk_ERROR_IHDR_INTERLACEMETHOD
	, Glass_Dechunk_ERROR_IHDR_NONSEQUENTIAL
	, Glass_Dechunk_ERROR_IHDR_EXTRA

	, Glass_Dechunk_ERROR_PLTE_ORDER
	, Glass_Dechunk_ERROR_PLTE_MISSING

	, Glass_Dechunk_ERROR_IDAT_MISSING
	, Glass_Dechunk_ERROR_DEFLATE_INCOMPLETE

	, Glass_Dechunk_ERROR_CHUNK_UNKNOWN_CRITICAL


	, Glass_Dechunk_Error_MAX
};

enum Glass_Defilter_Error {
	Glass_Defilter_Error_TYPE = Glass_ERRORTYPE_DEFILTER

	, Glass_Defilter_ERROR
	, Glass_Defilter_ERROR_EOTL

	, Glass_Defilter_ERROR_MALLOC

	, Glass_Defilter_ERROR_COLORTYPE
	, Glass_Defilter_ERROR_FILTERTYPE

};
#endif
