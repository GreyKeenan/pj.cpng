
#include "endian.h"


const union _Endian_Check _Endian_isLittle = { .doubleByte = 1 };

void Endian_flip(void *target, size_t size) {
	uint8_t *bytes = target;
	uint8_t buffer = 0;
	for (int i = 0; i < size / 2; ++i) {
		buffer = bytes[i];
		bytes[i] = bytes[size - 1 - i];
		bytes[size - 1 - i] = buffer;
	}
}
