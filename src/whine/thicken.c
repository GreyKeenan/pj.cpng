#include "./thicken.h"

#include "./easel.h"
#include "./canvas.h"
#include "./filters.h"
#include "./nofilter.h"

#include "gunc/log.h"
#include "gunc/iByteStream.h"
#include "gunc/iByteWriter.h"
#include "gunc/byteBalloon64.h"

#include <stdlib.h>

int Whine_thicken(const struct Whine_Easel *easel, struct Whine_Canvas *canvas, struct Gunc_iByteStream *bs) {

	int e = 0;

	if (canvas->status != Whine_Canvas_ABSENT) {
		Gunc_err("destination canvas not empty");
		return __LINE__;
	}

	if (easel->header.filterMethod != Whine_ImHeader_FILTER_DEFAULT) {
		Gunc_err("Unrecognized filter method: %d", easel->header.filterMethod);
		return __LINE__;
	}

	switch (easel->header.interlaceMethod) {
		case Whine_ImHeader_INTERLACE_NONE:
			break;
		case Whine_ImHeader_INTERLACE_ADAM7:
		default:
			Gunc_err("Unrecognized interlace method: %d", easel->header.interlaceMethod);
			return __LINE__;
	}

	Gunc_TODO("temporarily redirecting to old noFilter()");

	struct Gunc_iByteWriter bw = {0};
	struct Gunc_ByteBalloon64 bb = {0};

	e = Gunc_ByteBalloon64_init(&bb, &canvas->image, 1024);
	if (e) {
		Gunc_nerr(e, "failed to initialize bb");
		return __LINE__;
	}

	e = Gunc_ByteBalloon64_as_iByteWriter(&bb, &bw);
	if (e) {
		Gunc_nerr(e, "failed to init bw");
		return __LINE__;
	}

	e = Whine_nofilter(bs, &bw, easel);
	if (e) {
		Gunc_nerr(e, "Defiltering non-interlace failed");
		return __LINE__;
	}

	e = Gunc_ByteBalloon64_trim(&bb);
	if (e) {
		Gunc_nerr(e, "failed to trim bb");
		return __LINE__;
	}

	canvas->status = Whine_Canvas_SCANLINED;

	return 0;
}
