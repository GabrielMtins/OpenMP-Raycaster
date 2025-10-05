#include "Player.hpp"
#include "GlobalDef.hpp"

Player::Player(void) {
	angle = 0.0f;
	position = Vec2(3.0f, 3.0f);
	size = Vec2(0.5f, 0.5f);
}

void Player::handleInput(const Context& context, float dt) {
	const uint8_t *keys = SDL_GetKeyboardState(NULL);
	Vec2 wish_dir;

	angle += MOUSE_SENSITIVITY * dt * context.mouse_motion_x;
	direction = Vec2(cosf(angle), sinf(angle));

	if(keys[SDL_SCANCODE_A]) {
		wish_dir.y--;
	}
	
	if(keys[SDL_SCANCODE_D]) {
		wish_dir.y++;
	}

	if(keys[SDL_SCANCODE_W]) {
		wish_dir.x++;
	}
	
	if(keys[SDL_SCANCODE_S]) {
		wish_dir.x--;
	}

	if(keys[SDL_SCANCODE_J]) {
		angle -= TURN_SPEED * dt;
	}

	if(keys[SDL_SCANCODE_L]) {
		angle += TURN_SPEED * dt;
	}

	if(wish_dir.lengthSqr() > 0.8f) {
		velocity = wish_dir.normalized().rotate(direction) * PLAYER_SPEED;
	} else {
		velocity = Vec2::zero;
	}

}

bool Player::checkCollision(const Draw& map) {
	for(int i = 0; i < ceilf(size.x) + 1; i++) {
		for(int j = 0; j < ceilf(size.y) + 1; j++) {
			float mx = floorf(position.x) + i;
			float my = floorf(position.y) + j;

			if(!map.getPixel(mx, my)) {
				continue;
			}

			if(position.x + size.x < mx)
				continue;

			if(position.y + size.y < my)
				continue;

			if(position.x > mx + 1.0f)
				continue;

			if(position.y > my + 1.0f)
				continue;

			return true;
		}
	}

	return false;
}

void Player::applyMovement(const Draw& map, float dt) {
	Vec2 delta = velocity * dt;

	position.x += delta.x;

	if(checkCollision(map)) {
		position.x -= delta.x;
	}

	position.y += delta.y;

	if(checkCollision(map)) {
		position.y -= delta.y;
	}
} 

Vec2 Player::getCenter(void) const {
	return position + size / 2;
}
