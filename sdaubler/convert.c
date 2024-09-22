#include "./convert.h"
#include "./error.h"


int Sdaubler_convert(Sdaubler_iImageTrain *imt, SDL_Surface *destination) {
	SDL_Surface *surface = NULL;

	surface = SDL_CreateRGBSurface(
		0,
		1, 1, //w, h ... hm
		32,
		0xff000000, 0xff0000, 0xff00, 0xff
	);
	if (surface == NULL) {
		return Sdaubler_ERROR_CREATESURFACE;
	}


	//SDL_PixelFormat fmt = *surface->format;
	//printf("shiftes: 0x%x, 0x%x, 0x%x, 0x%x\n", fmt.Rshift, fmt.Gshift, fmt.Bshift, fmt.Ashift);


	// end
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	finalize:
	SDL_FreeSurface(surface);
	return 0;
}
