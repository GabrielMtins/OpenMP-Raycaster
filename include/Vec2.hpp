#ifndef VEC2_HPP
#define VEC2_HPP

#include <cmath>
#include <cstdio>

/**
 * Estrutura simples de Vec2 que utilizo nos meus projetos.
 * Os nomes dos métodos são autoexplicativos.
 */
struct Vec2 {
	float x, y;

	Vec2() : x(0.0f), y(0.0f) {}
	Vec2(float x, float y): x(x), y(y) {}

	inline Vec2 operator+(const Vec2& other) const {
		return Vec2(x + other.x, y + other.y);
	}

	inline Vec2 operator-(const Vec2& other) const {
		return Vec2(x - other.x, y - other.y);
	}

	inline Vec2 operator*(const Vec2& other) const {
		return Vec2(x * other.x, y * other.y);
	}

	inline Vec2 operator*(float val) const {
		return Vec2(x * val, y * val);
	}

	inline Vec2 operator/(float val) const {
		return Vec2(x / val, y / val);
	}

	inline Vec2& operator+=(const Vec2& other) {
		x += other.x;
		y += other.y;

		return *this;
	}

	inline Vec2& operator-=(const Vec2& other) {
		x -= other.x;
		y -= other.y;

		return *this;
	}

	inline Vec2& operator*=(const Vec2& other) {
		x *= other.x;
		y *= other.y;

		return *this;
	}

	inline Vec2& operator*=(float val) {
		x *= val;
		y *= val;

		return *this;
	}

	inline Vec2& operator/=(float val) {
		x /= val;
		y /= val;

		return *this;
	}

	inline bool operator==(const Vec2& other) {
		return other.x == x && other.y == y;
	}

	inline float lengthSqr(void) const {
		return x * x + y * y;
	}

	inline float length(void) const {
		return sqrtf(x * x + y * y);
	}

	inline Vec2 normalized(void) const {
		return *this / length();
	}

	inline float dot(const Vec2 &other) const {
		return x * other.x + y * other.y;
	}

	inline Vec2 rotate(const Vec2& direction) const {
		return Vec2(
				x * direction.x - y * direction.y,
				x * direction.y + y * direction.x
				);
	}

	inline void print(void) const {
		printf("%f %f\n", x, y);
	}

	static const Vec2 zero;
};

#endif
