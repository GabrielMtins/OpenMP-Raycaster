#ifndef DRAW_HPP
#define DRAW_HPP

#include <SDL2/SDL.h>
#include <string>

/**
 * Classe que contém uma textura que podemos definir os seus
 * pixels individualmente, por isso o nome, Draw (desenho).
 * É possível ler de um arquivo png usando o método load(),
 * ou criar uma tela nova para desenhar usando o método createScreen().
 */
class Draw {
	public:
		Draw(void);

		void createScreen(int width, int height);
		bool load(const std::string& filename);
		void destroy(void);

		void putPixel(int x, int y, uint32_t color);
		uint32_t getPixel(int x, int y) const;

		void clear(uint32_t color);

		SDL_Surface * getSurface(void);

		int getWidth(void);
		int getHeight(void);

		~Draw(void);

		bool checkBoundaries(int x, int y) const;

	private:
		SDL_Surface *surface;
		uint32_t *pixels;
};

#endif
