#include "Context.hpp"
#include "GlobalDef.hpp"
#include "SDL_video.h"
#include <SDL_image.h>

Context::Context(void) {
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);

	window = SDL_CreateWindow(
			"raycaster",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			WINDOW_WIDTH,
			WINDOW_HEIGHT,
			SDL_WINDOW_SHOWN
			);
	
	renderer = SDL_CreateRenderer(
			window,
			-1,
			SDL_RENDERER_ACCELERATED
			);

	SDL_SetRelativeMouseMode(SDL_TRUE);

	running = true;
}

void Context::pollEvents(void) {
	SDL_Event event;
	mouse_motion_x = 0;

	while(SDL_PollEvent(&event)) {
		switch(event.type) {
			case SDL_QUIT:
				running = false;
				break;

			case SDL_MOUSEMOTION:
				mouse_motion_x = event.motion.xrel;
				break;
		}
	}
}

bool Context::isRunning(void) const {
	return running;
}

void Context::updateWindowSurface(SDL_Surface *surface) {
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
	SDL_DestroyTexture(texture);
}

Context::~Context(void) {
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	IMG_Quit();
	SDL_Quit();
}
