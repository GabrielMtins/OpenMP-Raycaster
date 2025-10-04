#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Context.hpp"
#include "Vec2.hpp"
#include "Draw.hpp"

/**
 * Estrutura de Player, apenas responsável pela movimentação e 
 * colisão com o mundo, além de input. Não possui sistema de movimentação
 * avançado, pois não é o foco desse projeto.
 */
struct Player {
	Vec2 position;
	Vec2 velocity;
	Vec2 size;
	Vec2 direction;

	float angle;

	Player(void);

	void handleInput(const Context& context, float dt);

	bool checkCollision(const Draw& map);
	
	void applyMovement(const Draw& map, float dt);

	Vec2 getCenter(void) const;
};

#endif
