#ifndef AUTOPHAGICSEQUENCE_IMPL_H
#define AUTOPHAGICSEQUENCE_IMPL_H

struct AutophagicSequence {
	uint8_t *data;

	uint32_t cap;
	uint32_t length;

	uint32_t writePosition;
	uint32_t readPosition;
};

#endif
