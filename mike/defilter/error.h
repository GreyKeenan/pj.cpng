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
