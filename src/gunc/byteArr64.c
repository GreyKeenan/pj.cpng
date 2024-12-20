#include "./byteArr64.h"

#include "./log.h"

int Gunc_ByteArr64_look(struct Gunc_ByteArr64 *self, uint8_t *destination, int32_t at) {

	if (at >= 0) {
		if ((uint32_t)at >= self->length) {
			Gunc_err("looking beyond bounds. at: %d with length: %ld", at, self->length);
			return Gunc_iByteLooker_END;
		}
		*destination = self->data[at];

		return 0;
	}

	if ((uint32_t)(at * -1) > self->length) {
		Gunc_err("looking beyond bounds. at: %d with length: %ld", at, self->length);
		return Gunc_iByteLooker_END;
	}
	*destination = self->data[self->length + at];

	return 0;
}
