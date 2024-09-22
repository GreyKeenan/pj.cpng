#include "./convert.h"
#include "./error.h"

#include "./iImageTrain.h"

#include <stdlib.h>

int Sdaubler_convert(Sdaubler_iImageTrain *imt, SDL_Surface **destination) {
	SDL_Surface *surface = NULL;

	int e = 0;

	uint32_t width = 0; //TODO ERR: SDL_Surface width is 'int' not specified size.
	uint32_t height = 0;
	uint32_t pixel = 0;

	uint64_t length = 0;

	if (Sdaubler_iImageTrain_gauge(imt, &width, &height)) {
		return Sdaubler_ERROR_CONVERT_GAUGE;
	}

	surface = SDL_CreateRGBSurface(
		0,
		width, height,
		32,
		0xff000000, 0xff0000, 0xff00, 0xff
	);
	if (surface == NULL) {
		e = Sdaubler_ERROR_CONVERT_SURFACE_CREATE;
		goto finalize;
	}

	// populate
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	length = (uint64_t)width * height;
	for (uint64_t i = 0; i < length; ++i) {
		if (Sdaubler_iImageTrain_choochoo(imt, &pixel)) {
			e = Sdaubler_ERROR_CONVERT_READPIXEL;
			goto finalize;
		}

		((uint32_t*)(surface->pixels))[i] = pixel;
	}

	*destination = surface;
	surface = NULL;

	// end
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	finalize:
	if (surface != NULL) {
		SDL_FreeSurface(surface);
	}
	return e;
}
