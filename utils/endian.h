#ifndef ENDIAN_H
#define ENDIAN_H

#include <stddef.h>
#include <stdint.h>

union _Endian_Check {
	uint8_t singleByte;
	uint16_t doubleByte;
};

extern const union _Endian_Check _Endian_isLittle;
#define ENDIAN_ISLITTLE _Endian_isLittle.singleByte

void Endian_flip(void *target, size_t size);

#endif
