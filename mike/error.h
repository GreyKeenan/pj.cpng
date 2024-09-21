#ifndef Mike_ERROR_H
#define Mike_ERROR_H

enum Mike_Error {
	Mike_ERROR = 1

	, Mike_ERROR_EOTL
	, Mike_ERROR_NOTPNG

};

#define Mike_ERRORTYPE_DECHUNK 0x2000
#define Mike_ERRORTYPE_DECOMPRESS 0x4000
#define Mike_ERRORTYPE_DEFILTER 0x6000
/*
	easy way to ensure errors are unique btwn these sections
	subject to change

*/


#endif
