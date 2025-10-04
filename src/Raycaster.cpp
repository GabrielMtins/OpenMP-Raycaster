#include "Raycaster.hpp"
#include <cmath>

#include <omp.h>

static const Vec2 infinite_vec = Vec2(10000.0f, 10000.0f);

Raycaster::Raycaster(void) {
	screen.createScreen(SCREEN_WIDTH, SCREEN_HEIGHT);
	map_wall.createScreen(WORLD_SIZE, WORLD_SIZE);
	map_ceilling.createScreen(WORLD_SIZE, WORLD_SIZE);
	map_floor.createScreen(WORLD_SIZE, WORLD_SIZE);

	textures.load("textures.png");

	map_ceilling.clear(1);
	map_floor.clear(1);

	for(int i = 0; i < 10; i++) {
		for(int j = 0; j < 10; j++) {
			if(i + j < 8) {
				map_floor.putPixel(i, j, 3);
			}
		}
	}

	for(int i = 0; i < 10; i++) {
		map_wall.putPixel(i, 0, 2);
		map_wall.putPixel(0, i, 2);
		map_wall.putPixel(i, WORLD_SIZE - 1, 2);
		map_wall.putPixel(WORLD_SIZE - 1, i, 2);
	}

	map_wall.putPixel(1, 1, 2);

	map_wall.putPixel(5, 5, 2);
	map_wall.putPixel(5, 6, 2);

	camera.position = Vec2(5.0f, 3.0f);
	camera.angle = 0.0f;
}

Draw& Raycaster::getScreen(void) {
	return screen;
}

void Raycaster::processScreen(void) {
	int i;

	screen.clear(0x000000ff);

	/**
	 * Esse deve ser o vetor que representa a direção da câmera.
	 * Utilizaremos ele para calcular o tamanho da parede.
	 */
	camera.direction = Vec2(cosf(camera.angle), sinf(camera.angle));

	/**
	 * Aqui, chamamos o openmp para realizar a otimização do for.
	 * Ele irá paralelizar o for, aumentando bastante o desempenho.
	 */
	#pragma omp parallel for 
	for(i = 0; i < SCREEN_WIDTH; i++) {
		processWall(i);
	}
}

void Raycaster::processWall(int index) {
	WallInformation information = findClosestWall(index);

	if(information.position == infinite_vec)
		return;

	/**
	 * Se utilizássemos a distância padrão entre a posição da parede a posição da câmera,
	 * teríamos um efeito de olho de peixe. Para conseguir a distância real, devemos
	 * Calcular o produto escalar entre o vetor da diferença das posições da câmera e da 
	 * parede e o vetor que representa a direção da câmera.
	 */
	float distance = (information.position - camera.position).dot(camera.direction);

	/**
	 * A altura da parede é inversamente proporcional a distância, por isso,
	 * a altura é dada pela divisão de uma constante e a distância.
	 * Utilizamos a largura da tela dividida por 2 como constante para 
	 * que as texturas não fiquem achatadas.
	 */
	int height = (float) SCREEN_WIDTH / distance / 2;

	drawWall(
			index,
			SCREEN_HEIGHT/2 - height/2,
			SCREEN_HEIGHT/2 + height/2,
			information
			);

	/**
	 * Chamando a função com is_floor == true para
	 * que as texturas sampleadas sejam as adequadas
	 */
	drawPlane(index,
			SCREEN_HEIGHT/2 + height/2,
			SCREEN_HEIGHT,
			true
			);

	/**
	 * Chamando a função com is_floor == false para
	 * que as texturas sampleadas sejam as adequadas
	 */
	drawPlane(index,
			0,
			SCREEN_HEIGHT/2 - height/2,
			false
			);
}

void Raycaster::drawWall(int x, int start_y, int end_y, const WallInformation& information) {
	int i;
	int x_image, y_image;

	/**
	 * Utilizamos a informação sobre a interseção (se foi no eixo x ou não)
	 * para saber como devemos fazer o sampling da textura.
	 */
	if(!information.x_intersection) {
		y_image = (information.position.x - floorf(information.position.x)) * textures.getWidth();
	} else {
		y_image = (information.position.y - floorf(information.position.y)) * textures.getWidth();
	}
	
	/**
	 * Aqui, iremos deslocar verticalmente de acordo com a textura que escolhemos.
	 */
	y_image += information.texture_index * textures.getWidth();

	for(i = start_y; i < end_y; i++) {
		/**
		 * Note, estamos calculando a interpolação linear no eixo "x". Por conta disso,
		 * as texturas serão rotacionadas 90°. Para resolver isso, as texturas já devem
		 * estar previamente rotacionadas -90°, para que elas sejam exibidas corretamente.
		 * Esse comportamento se deve a otimização do uso de cache, pois se a interpolação
		 * linear estivesse no eixo Y, haveria cache miss em toda sampling de textura,
		 * já que os dados estariam distantes.
		 */
		x_image = textures.getWidth() * (i - start_y) / (end_y - start_y);
		
		screen.putPixel(
				x,
				i,
				textures.getPixel(x_image, y_image)
				);
	}
}

void Raycaster::drawPlane(int x, int start_y, int end_y, bool is_floor) {
	int i;
	int x_image, y_image;
	Vec2 floor_pos;
	Draw& map_used = is_floor ? map_floor : map_ceilling;
	int texture_index;

	/**
	 * Nesse for, não temos muita possibilidade de otimização.
	 * Haverá cache miss independente do que queiramos, é puramente
	 * aleatório.
	 */
	for(i = start_y; i < end_y; i++) {
		floor_pos = getFloorCoordinates(x, i);
		texture_index = map_used.getPixel(floor_pos.x, floor_pos.y) - 1;

		x_image = (floor_pos.x - floorf(floor_pos.x)) * textures.getWidth();
		y_image = (floor_pos.y - floorf(floor_pos.y) + texture_index) * textures.getWidth();

		screen.putPixel(
				x,
				i,
				textures.getPixel(x_image, y_image)
				);
	}
}

Vec2 Raycaster::getFloorCoordinates(int x_screen, int y_screen) {
	/**
	 * O comportamento dessa função é bem legal. Quando trabalhando com projeções,
	 * temos:
	 * x_norm = x / z
	 * y_norm = y / z
	 * Onde x_norm e y_norm são as coordenadas normalizadas no espaço da tela e (x, y, z)
	 * são as coordenadas no espaço euclidiano, com z sendo a profundidade.
	 *
	 * Note que, se soubermos o valor de y, a altura, conseguimos saber o valor de z e 
	 * também de x.
	 * Vamos chamar z de depth e y de FLOOR_HEIGHT:
	 *
	 * y_norm = FLOOR_HEIGHT / depth
	 * depth = FLOOR_HEIGHT / y_norm
	 * Portanto,
	 *
	 * x_norm = x / depth
	 * x = depth * x_norm
	 * É dessa forma que essa função calcula as coordenadas do chão dado
	 * um pixel qualquer da tela. 
	 * Óbviamente, depois disso devemos aplicar as rotações e translações
	 * para obter a coordenada transformada.
	 */
	float x_screen_norm = (float) (x_screen - SCREEN_WIDTH / 2) / SCREEN_WIDTH * 2;
	float y_screen_norm = (float) (y_screen - SCREEN_HEIGHT / 2) / SCREEN_WIDTH * 2;

	float depth = FLOOR_HEIGHT / y_screen_norm;

	if(y_screen_norm > 0.0f) {
		depth *= -1.0f;
	}

	Vec2 floor = Vec2(
			depth,
			x_screen_norm * depth
			);

	floor = floor.rotate(Vec2(camera.direction));

	floor += camera.position;

	return floor;
}

WallInformation Raycaster::findClosestWall(int index) const {
	/**
	 * Geralmente, alguns raycasters utilizam um algoritmo de DDA, como
	 * o que está no site do lodev. Porém, eu acho ele um pouco complicado de entender,
	 * eu optei por procurar as intersecções no eixo X e no eixo Y e depois
	 * retornar a mais próxima da câmera.
	 */
	WallInformation wall_information;
	Vec2 direction = Vec2(
			1.0f,
			(float) (index - SCREEN_WIDTH / 2) / (SCREEN_WIDTH) * 2.0f
			);

	direction = direction.rotate(camera.direction);

	Vec2 wall_x = findClosestWallX(direction);
	Vec2 wall_y = findClosestWallY(direction);

	float distance_sqr_x = (wall_x - camera.position).lengthSqr();
	float distance_sqr_y = (wall_y - camera.position).lengthSqr();

	if(distance_sqr_x < distance_sqr_y) {
		wall_information.position = wall_x;
		wall_information.x_intersection = true;
	} else {
		wall_information.position = wall_y;
		wall_information.x_intersection = false;
	}

	wall_information.texture_index = map_wall.getPixel(
			wall_information.position.x,
			wall_information.position.y
			) - 1;

	return wall_information;
}

Vec2 Raycaster::findClosestWallX(const Vec2& direction) const {
	Vec2 start = camera.position;
	Vec2 first_delta;
	Vec2 delta;

	if(direction.x == 0.0f) {
		return infinite_vec;
	}

	if(direction.x > 0.0f) {
		first_delta.x = floorf(start.x + 1) - start.x;
	} else {
		first_delta.x = floorf(start.x) - start.x;
	}

	/**
	 * Aqui não tem muito mistério, é mais porque a câmera pode estar
	 * num meio de um tile, como a posição (1.73, 2.25). Se quisemos apenas
	 * iterar adicionando 1 em um dos eixos, devemos calcular a primeira
	 * intersecção como o eixo desejado.
	 */
	first_delta.y = direction.y / direction.x * first_delta.x;

	start += first_delta;

	delta = direction / fabsf(direction.x);

	return findClosestWallIterate(start, delta);
}

Vec2 Raycaster::findClosestWallY(const Vec2& direction) const {
	Vec2 start = camera.position;
	Vec2 first_delta;
	Vec2 delta;

	if(direction.y == 0.0f) {
		return infinite_vec;
	}

	if(direction.y > 0.0f) {
		first_delta.y = floorf(start.y + 1) - start.y;
	} else {
		first_delta.y = floorf(start.y) - start.y;
	}

	first_delta.x = direction.x / direction.y * first_delta.y;

	start += first_delta;

	delta = direction / fabsf(direction.y);
	
	return findClosestWallIterate(start, delta);
}

Vec2 Raycaster::findClosestWallIterate(Vec2 start, const Vec2& delta) const {
	/**
	 * Isso aqui é um "hack" apenas para facilitar o código. Se o
	 * delta.x < 0, eu teria que verificar o quadrado à esquerda
	 * da intersecção. Esse "hack" apenas facilita o código, pois
	 * antes eu teria que fazer ((int) x - 1), e agora basta fazer
	 * ((int) x); Exemplo:
	 * x = 3.0
	 * ((int) 3.0 - 1) = 2
	 * x = 2.99
	 * ((int) 2.99) = 2
	 */
	if(delta.x < 0.0f) {
		start.x -= 0.01f;
	}
	if(delta.y < 0.0f) {
		start.y -= 0.01f;
	}

	while(1) {
		int map_x, map_y;

		map_x = start.x;
		map_y = start.y;

		if(!map_wall.checkBoundaries(map_x, map_y)) {
			break;
		}

		if(map_wall.getPixel(map_x, map_y)) {
			return start;
		}

		start += delta;
	}

	return infinite_vec;
}
