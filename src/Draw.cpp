#include "Draw.hpp"
#include <SDL2/SDL_image.h>

Draw::Draw(void) {
	surface = NULL;
	pixels = NULL;
}

void Draw::createScreen(int width, int height) {
	surface = SDL_CreateRGBSurfaceWithFormat(
			0,
			width,
			height,
			32,
			SDL_PIXELFORMAT_RGBA32
			);

	pixels = (uint32_t *) surface->pixels;
}

bool Draw::load(const std::string& filename) {
	surface = IMG_Load(filename.c_str());
	pixels = (uint32_t *) surface->pixels;

	return surface != NULL;
}

void Draw::destroy(void) {
	if(surface != NULL) {
		SDL_FreeSurface(surface);
	}
}

void Draw::putPixel(int x, int y,uint32_t color) {
	if(!checkBoundaries(x, y))
		return;

	pixels[x + y * surface->w] = color;
}

uint32_t Draw::getPixel(int x, int y) const {
	if(!checkBoundaries(x, y))
		return 0;

	return pixels[x + y * surface->w];
}

void Draw::clear(uint32_t color) {
	SDL_FillRect(surface, NULL, color);
}

SDL_Surface * Draw::getSurface(void) {
	return surface;
}

int Draw::getWidth(void) {
	return surface->w;
}

int Draw::getHeight(void) {
	return surface->h;
}

Draw::~Draw(void) {
	destroy();
}

bool Draw::checkBoundaries(int x, int y) const {
	return (x >= 0) && (y >= 0) && (x < surface->w) && (y < surface->h);
}
