#ifndef EXPANDINGWRITER_IMPL_H
#define EXPANDINGWRITER_IMPL_H

#include <stdint.h>

struct ExpandingWriter {
	uint8_t *nData;
	uint32_t writePosition;
	uint32_t cap;
	uint16_t step;
};

#endif
