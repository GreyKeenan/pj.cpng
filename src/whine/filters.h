#ifndef WHINE_filters_H
#define WHINE_filters_H

#include <stdlib.h>
#include <stdint.h>

static inline uint8_t Whine_filt_a(uint32_t wi, const uint8_t *scanlineBuffer, uint8_t bytesPerPixel);
static inline uint8_t Whine_filt_b(uint32_t wi, const uint8_t *scanlineBuffer);
static inline uint8_t Whine_filt_c(uint64_t cBuffer, uint8_t bytesPerPixel);
static inline uint8_t Whine_paeth(int a, int b, int c);


static inline uint8_t Whine_filt_a(uint32_t wi, const uint8_t *scanlineBuffer, uint8_t bytesPerPixel) {
	if (wi < bytesPerPixel) {
		return 0;
	}
	return scanlineBuffer[wi - bytesPerPixel];
}
static inline uint8_t Whine_filt_b(uint32_t wi, const uint8_t *scanlineBuffer) {
	return scanlineBuffer[wi];
}
static inline uint8_t Whine_filt_c(uint64_t cBuffer, uint8_t bytesPerPixel) {
	cBuffer >>= 8 * (bytesPerPixel - 1);
	return cBuffer & 0xff;
}
static inline uint8_t Whine_paeth(int a, int b, int c) {
	// https://www.w3.org/TR/2003/REC-PNG-20031110/#9Filter-type-4-Paeth
	int p = a + b - c;
	int pa = abs(p - a);
	int pb = abs(p - b);
	int pc = abs(p - c);

	if (pa <= pb && pa <= pc) {
		return a;
	}
	if (pb <= pc) {
		return b;
	}
	return c;
}

#endif
