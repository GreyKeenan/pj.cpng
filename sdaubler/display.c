#include "./display.h"
#include "./error.h"

#include "SDL2/SDL.h"

void sdaubler_loop(void);

int Sdaubler_display(iPixelSequence *ps) {

	int e = 0;

	SDL_DisplayMode displayMode = {0};

	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;

	// convert image
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// ...

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

	// ...

	// stay open
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	sdaubler_loop();

	// end
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	finalize:
	if (window != NULL) {
		SDL_DestroyWindow(window);
	}
	if (renderer != NULL) {
		SDL_DestroyRenderer(renderer);
	}
	SDL_Quit();
	return e;
}

void sdaubler_loop(void) {
	SDL_Event event = {0};
	while (1) {
		if (SDL_PollEvent(&event) == 0) {
			continue;
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
	return;
}
