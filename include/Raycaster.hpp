#ifndef RAYCASTER_HPP
#define RAYCASTER_HPP

#include "Draw.hpp"
#include "Vec2.hpp"
#include "GlobalDef.hpp"

#include <array>

/** 
 * Estrutura que possui as informações
 * das intersecções das paredes com a semirreta
 * da direção da camera.
 */
struct WallInformation {
	Vec2 position;

	/**
	 * Se for verdadeiro, indica que a intersecção
	 * ocorreu no eixo X, caso contrário, no eixo Y.
	 */
	bool x_intersection;

	/**
	 * Index de textura, se é a textura 0, 1, 2 etc.
	 * As texturas devem estar carregadas verticalmente
	 * num mapa de texturas único. Elas devem estar carregadas
	 * verticalmente para diminuir a quantidade de cache misses.
	 */
	int texture_index;
};

/**
 * Classe responsável pela renderização usando
 * raycasting per se.
 */
class Raycaster {
	public:
		Raycaster(void);
		Draw& getScreen(void);
		void processScreen(void);

		struct {
			Vec2 position;
			Vec2 direction;
			float angle;
		} camera;

		/**
		 * Como a classe Draw é só uma superfície que podemos definir 
		 * os valores, tomei a liberdade de utilizá-la como um mapa também,
		 * para evitar a criação de uma nova classe, que teria métodos
		 * bem parecidos
		 */
		Draw map_wall;
		Draw map_floor;
		Draw map_ceilling;

	private:
		/**
		 * Essa função procura a parede mais próxima dado um índice x
		 * da tela, calcula a altura e chama as funções de renderizar
		 * o chão e o teto
		 */
		void processWall(int index);

		/** 
		 * Essa função renderiza a parede, dado o início e o fim dela no eixo y.
		 * Ela também interpola as texturas, baseadas nesses dados.
		 */
		void drawWall(int x, int start_y, int end_y, const WallInformation& information);

		/**
		 * Renderiza um plano horizontal. A variável is_floor serve para
		 * indicar se estamos renderizando o chão ou o teto.
		 */
		void drawPlane(int x, int start_y, int end_y, bool is_floor);

		/**
		 * Função que dado dois pixels na tela, calcula a profundidade
		 * para achar a coordenada adequada do chão.
		 */
		Vec2 getFloorCoordinates(int x_screen, int y_screen);

		/**
		 * Função que retorna a parede mais próxima, dado um índice (coordenada x da tela).
		 */
		WallInformation findClosestWall(int index) const;
		/**
		 * Função que retorna a intersecção mais pŕoxima da semirreta dada a direção
		 * em relação ao eixo X.
		 */
		Vec2 findClosestWallX(const Vec2& direction) const;
		/**
		 * Função que retorna a intersecção mais pŕoxima da semirreta dada a direção
		 * em relação ao eixo Y.
		 */
		Vec2 findClosestWallY(const Vec2& direction) const;
		/**
		 * Função que itera entre um ponto de início utilizando um delta até
		 * achar uma intersecção.
		 */
		Vec2 findClosestWallIterate(Vec2 start, const Vec2& delta) const;

		/**
		 * Região que representa a tela, onde renderizaremos o raycasting.
		 */
		Draw screen;

		/**
		 * Todas as texturas devem ser carregadas numa única textura, onde
		 * elas são armazenadas verticalmente, uma abaixo da outra. Com isso,
		 * diminui-se a chance de cache misses (embora, ainda seja alta).
		 */
		Draw textures;
};

#endif
