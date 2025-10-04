#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <SDL2/SDL.h>

/**
 * Classe base, apenas para criar uma janela
 * e atualizar os eventos que ocorrem nela. Estaremos
 * usando o SDL2 para isso.
 */
class Context {
	public:
		Context(void);

		void pollEvents(void);

		bool isRunning(void) const;

		void updateWindowSurface(SDL_Surface *surface);

		~Context(void);

		int mouse_motion_x;

	private:
		SDL_Window *window;
		SDL_Renderer *renderer;
		bool running;
};

#endif
