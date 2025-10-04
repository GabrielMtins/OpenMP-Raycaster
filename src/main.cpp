#include "Context.hpp"
#include "Draw.hpp"
#include "Vec2.hpp"
#include "Raycaster.hpp"
#include "Player.hpp"

#include <cmath>

#include <omp.h>

const Vec2 Vec2::zero = Vec2(0.0f, 0.0f);

static void run_args(int argc, char **argv) {
	for(int i = 1; i < argc; i++) {
		if(!strcmp(argv[i], "--single")) {
			omp_set_num_threads(1);
		}
	}
}

/**
 * A função main que possui todas as variáveis e objetos para
 * funcionar. 
 * Se quiser saber como o raycasting em si funciona, basta ler
 * o Raycaster.hpp e o Raycaster.cpp
 */
int main(int argc, char **argv) {
	uint32_t start_tick;
	float dt = 0.00f;;

	run_args(argc, argv);

	uint32_t first_tick = SDL_GetTicks();
	uint32_t final_tick;
	uint32_t counter = 0;

	Context context;
	Raycaster raycaster;
	Player player;

	while(context.isRunning()) {
		start_tick = SDL_GetTicks();

		/**
		 * Atualiza os eventos, input do player, e
		 * renderiza para a tela... */
		context.pollEvents();
		player.handleInput(context, dt);
		player.applyMovement(raycaster.map_wall, dt);

		/**
		 * Atualizar as posições da câmera e dos ângulos
		 * segundo a posição do player */
		raycaster.camera.position = player.getCenter();
		raycaster.camera.angle = player.angle;

		/* Renderizar na tela */
		raycaster.processScreen();
		context.updateWindowSurface(raycaster.getScreen().getSurface());

		dt = (float) (SDL_GetTicks() - start_tick) / 1000;
		
		counter++;
	}

	final_tick = SDL_GetTicks();
	float delta = (float) (final_tick - first_tick) / 1000.0f;

	printf("Program time: %f\n", delta);
	printf("Total frames: %u\n", counter);
	printf("Average FPS: %f\n", (float) counter / delta);

	return 0;
}
