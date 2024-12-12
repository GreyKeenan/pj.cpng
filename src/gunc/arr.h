#ifndef GUNC_Arr_H
#define GUNC_Arr_H

#include <stdint.h>
#include <stddef.h>

#define Gunc_Arr_nfree(arr) \
	if ((arr)->data != NULL) { \
		free((arr)->data); \
		(arr)->data = NULL; \
	} \
	(arr)->length = 0

//struct Gunc_ByteArr16 { uint8_t *data; uint16_t length; };
struct Gunc_ByteArr32 { uint8_t *data; uint32_t length; };
struct Gunc_ByteArr64 { uint8_t *data; uint64_t length; };
//struct Gunc_SByteArr16 { int8_t *data; uint16_t length; };
struct Gunc_SByteArr32 { int8_t *data; uint32_t length; };
struct Gunc_SByteArr64 { int8_t *data; uint64_t length; };

//struct Gunc_ShortArr16 { uint16_t *data; uint16_t length; };
struct Gunc_ShortArr32 { uint16_t *data; uint32_t length; };
struct Gunc_ShortArr64 { uint16_t *data; uint64_t length; };
//struct Gunc_SShortArr16 { int16_t *data; uint16_t length; };
struct Gunc_SShortArr32 { int16_t *data; uint32_t length; };
struct Gunc_SShortArr64 { int16_t *data; uint64_t length; };

//struct Gunc_RuneArr16 { uint32_t *data; uint16_t length; };
struct Gunc_RuneArr32 { uint32_t *data; uint32_t length; };
struct Gunc_RuneArr64 { uint32_t *data; uint64_t length; };
//struct Gunc_SRuneArr16 { int32_t *data; uint16_t length; };
struct Gunc_SRuneArr32 { int32_t *data; uint32_t length; };
struct Gunc_SRuneArr64 { int32_t *data; uint64_t length; };

//struct Gunc_LongArr16 { uint64_t *data; uint16_t length; };
struct Gunc_LongArr32 { uint64_t *data; uint32_t length; };
struct Gunc_LongArr64 { uint64_t *data; uint64_t length; };
//struct Gunc_SLongArr16 { int64_t *data; uint16_t length; };
struct Gunc_SLongArr32 { int64_t *data; uint32_t length; };
struct Gunc_SLongArr64 { int64_t *data; uint64_t length; };

#endif
