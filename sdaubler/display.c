#include "./display.h"
#include "./error.h"

#include "./convert.h"

#include "SDL2/SDL.h"

int sdaubler_loop(void);

int Sdaubler_display(Sdaubler_iImageTrain *imt) {

	int e = 0;

	SDL_Surface *surface = NULL;
	SDL_Texture *texture = NULL;

	SDL_Rect rect = {0};

	SDL_DisplayMode displayMode = {0};

	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;

	// convert image
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	e = Sdaubler_convert(imt, &surface);
	if (e) return e;

	// init SDL
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	e = SDL_InitSubSystem(SDL_INIT_VIDEO);
	if (e) {
		return Sdaubler_ERROR_SDL_INIT;
	}

	e = SDL_GetCurrentDisplayMode(0, &displayMode);
	if (e) {
		displayMode.w = 500;
		displayMode.h = 500;
	}
	window = SDL_CreateWindow(
		"Decoded PNG!",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		displayMode.w >> 1, displayMode.h >> 1,
		0
		
	);
	if (window == NULL) {
		e = Sdaubler_ERROR_WINDOW_INIT;
		goto finalize;
	}
	
	renderer = SDL_CreateRenderer(window, -1, 0);
	if (renderer == NULL) {
		e = Sdaubler_ERROR_RENDERER_INIT;
		goto finalize;
	}

	// render image
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (texture == NULL) {
		e = Sdaubler_ERROR_TEXTURE_INIT;
		goto finalize;
	}

	SDL_FreeSurface(surface);
	surface = NULL;

	rect = (SDL_Rect){0,0, displayMode.h >> 1, displayMode.h >> 1};

	e = SDL_RenderCopy(renderer, texture, NULL, &rect);
	if (e) {
		e = Sdaubler_ERROR_RENDERCOPY;
		goto finalize;
	}

	SDL_RenderPresent(renderer);

	// stay open
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	e = sdaubler_loop();
	if (e) goto finalize;

	// end
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	finalize:
	if (surface != NULL) {
		SDL_FreeSurface(surface);
	}
	if (texture != NULL) {
		SDL_DestroyTexture(texture);
	}
	if (window != NULL) {
		SDL_DestroyWindow(window);
	}
	if (renderer != NULL) {
		SDL_DestroyRenderer(renderer);
	}
	SDL_Quit(); //segmentation fault happens here when SDl_RenderPresent is not called ????
	return e;
}

int sdaubler_loop(void) {
	SDL_Event event = {0};
	while (1) {
		if (!SDL_WaitEvent(&event)) {
			return Sdaubler_ERROR_WAITEVENT;
		}
		switch (event.type) {
			case SDL_QUIT:
				goto endwhile;
				break;
			default:
				continue;
		}
	}
	endwhile:
	return 0;
}
