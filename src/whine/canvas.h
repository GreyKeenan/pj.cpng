#ifndef WHINE_Canvas_H
#define WHINE_Canvas_H

#include "gunc/arr.h"

#include <stdint.h>

enum Whine_Canvas_dataStatus {
	Whine_Canvas_ABSENT = 0
	, Whine_Canvas_COMPRESSED
	, Whine_Canvas_FILTERED
	, Whine_Canvas_SCANLINED
};

struct Whine_Canvas {
	struct Gunc_ByteArr64 image;
	enum Whine_Canvas_dataStatus status;
};

void Whine_Canvas_filicide(struct Whine_Canvas *self);

#endif
