#include <stdio.h>

#include <stdint.h>

int Puff_measureLengthSymbol(uint16_t length, uint16_t *baseValue, uint8_t *numExtraBits);
int Puff_measureDistanceSymbol(uint8_t distance, uint16_t *baseValue, uint8_t *numExtraBits);

int main() {

	int e = 0;

	uint16_t base = 0;
	uint8_t extraBits = 0;

	/*
	for (uint16_t i = 0; i < 300; ++i) {

		e = Puff_measureLengthSymbol(i, &base, &extraBits);

		printf("[%d]: (e%d) (%d, %d)\n",
			i, e, base, extraBits
		);

	}
	*/

	for (uint16_t i = 0; i < 32; ++i) {
		e = Puff_measureDistanceSymbol(i, &base, &extraBits);

		printf("[%d]: (e%d) (%d, %d)\n",
			i, e, base, extraBits
		);
	}


}
