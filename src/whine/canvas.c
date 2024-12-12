#include "./canvas.h"

#include <stdlib.h>

void Whine_Canvas_filicide(struct Whine_Canvas *self) {

	Gunc_Arr_nfree(&self->image);
	self->status = Whine_Canvas_ABSENT;

	return;
}
