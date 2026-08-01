// Vector2D.h

#pragma once

#include <cmath>

struct Vector2D {
	float x;
	float y;

	Vector2D(float _x = 0.0f, float _y = 0.0f) : x(_x), y(_y) {}

	// 다른 벡터와의 연산
	Vector2D operator+(const Vector2D& v2) const{
		return Vector2D{(x+v2.x), (y+v2.y)};
	}
	Vector2D operator-(const Vector2D& v2) const {
		return Vector2D{ (x - v2.x), (y - v2.y) };
	}

	// 자신의 상태가 변하는 연산
	Vector2D& operator+=(const Vector2D& v2) {
		x += v2.x;
		y += v2.y;
		return* this;
	}
	Vector2D& operator-=(const Vector2D& v2) {
		x -= v2.x;
		y -= v2.y;
		return* this;
	}

	// 다른 값(scala)과의 연산
	Vector2D operator*(float scala) const{
		return Vector2D{ (x *scala), (y *scala) };
	}
	Vector2D operator/(float scala) const{
		return Vector2D{ (x / scala), (y / scala) };
	}
	Vector2D& operator*=(float scala) {
		x *= scala;
		y *= scala;
		return* this;
	}
	Vector2D& operator/=(float scala) {
		x /= scala;
		y /= scala;
		return* this;
	}

	// 내적. 두 힘이 향하는 곳이 비슷한 정도를 반환
	float Dot(const Vector2D& v) const {
		return x * v.x + y * v.y;
	}

	// 피타고라스 정리를 통해 벡터가 표현하는 물리량(유클리드)을 얻기
	float Length() const {
		return std::sqrt((x * x) + (y * y));
	}
	// 연산 비용을 아끼과 위해 제곱된 길이값을 구하는 함수. 벡터의 크기를 비교할 때 유용하게 사용될 수 있음.
	float LengthSquared() const {
		return (x * x) + (y * y);
	}

	// 물리량과는 별개로 방향을 구하기. 물리량(유클리드)을 1로 고정 후 방향에 대한 값만 구하는 것.
	Vector2D Normalize() const {
		float len = Length();
		// 0으로 나누지 않도록 하기
		return (len > 0) ? *this / len : Vector2D(0, 0);
	}

	// 벡터가 가진 값이 모두 0인지 확인하는 함수
	bool IsZero() const {
		return x == 0.0 && y == 0.0;
	}
};