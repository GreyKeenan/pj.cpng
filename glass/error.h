#ifndef GLASS_ERROR_H
#define GLASS_ERROR_H

enum Glass_decode_Error {
	Glass_decode_ERROR = 1

	, Glass_decode_ERROR_IMPOSSIBLE

	, Glass_decode_ERROR_EOTL
	, Glass_decode_ERROR_NOTPNG



	, Glass_decode_Error_top
};

enum Glass_dechunk_Error {
	Glass_dechunk_ERROR = Glass_decode_Error_top

	, Glass_dechunk_ERROR_INT32
	, Glass_dechunk_ERROR_EOTL

	, Glass_dechunk_ERROR_IHDR_NOT
	, Glass_dechunk_ERROR_IHDR_LENGTH
	, Glass_dechunk_ERROR_IHDR_COLORTYPE
	, Glass_dechunk_ERROR_IHDR_BITDEPTH
	, Glass_dechunk_ERROR_IHDR_COMPRESSIONMETHOD
	, Glass_dechunk_ERROR_IHDR_FILTERMETHOD
	, Glass_dechunk_ERROR_IHDR_INTERLACEMETHOD
	, Glass_dechunk_ERROR_IHDR_NONSEQUENTIAL
	, Glass_dechunk_ERROR_IHDR_EXTRA

	, Glass_dechunk_ERROR_PLTE_ORDER
	, Glass_dechunk_ERROR_PLTE_MISSING

	, Glass_dechunk_ERROR_IDAT_MISSING
	, Glass_dechunk_ERROR_DEFLATE_INCOMPLETE

	, Glass_dechunk_ERROR_CHUNK_UNKNOWN_CRITICAL



	, Glass_dechunk_Error_top

};

enum Glass_defilter_Error {
	Glass_defilter_ERROR = Glass_dechunk_Error_top

	, Glass_defilter_ERROR_EOTL

	, Glass_defilter_ERROR_MALLOC

	, Glass_defilter_ERROR_COLORTYPE
	, Glass_defilter_ERROR_FILTERTYPE

};

#endif
