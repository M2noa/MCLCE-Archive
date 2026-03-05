/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

#include "network/BinaryStream.h"

class Vec2 {
public:

	static const Vec2 ZERO, ONE, UNIT_X, NEG_UNIT_X, UNIT_Y, NEG_UNIT_Y, MAX, MIN;

	float x, y;

	Vec2(float _x, float _y) :
		x(_x)
		, y(_y) {

	}

	Vec2() :
		Vec2(0.f, 0.f) {

	}

	Vec2(const FVector2D& rhs)
		: Vec2(rhs.X, rhs.Y) {
	}

	operator FVector2D() const {
		return FVector2D(x, y);
	}

	Vec2 operator*(float s) const {
		return Vec2(x * s, y * s);
	}

	float operator*(const Vec2& v) const {
		return x * v.x + y * v.y;
	}

	Vec2 operator/(float k) const {
		return (*this) * (1.f / k);
	}

	Vec2 operator-(const Vec2& rhs) const {
		return Vec2(x - rhs.x, y - rhs.y);
	}

	Vec2 operator+(const Vec2& rhs) const {
		return Vec2(x + rhs.x, y + rhs.y);
	}

	Vec2& operator+=(const Vec2& rhs) {
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	Vec2& operator*=(float k) {
		x *= k;
		y *= k;
		return *this;
	}

	bool operator==(const Vec2& rhs) const {
		return x == rhs.x && y == rhs.y;
	}

	bool operator!=(const Vec2& rhs) const {
		return x != rhs.x || y != rhs.y;
	}

	Vec2 normalized() const {
		const float dist = sqrt(x * x + y * y);
		return (dist < 0.0001f) ? Vec2::ZERO : Vec2(x / dist, y / dist);// newTemp
	}

	float dot(const Vec2& p) const {
		return x * p.x + y * p.y;
	}

	float length() const {
		return sqrt(x * x + y * y);
	}

	float lengthSquared() const {
		return x * x + y * y;
	}

	float distanceToSqr(const Vec2& p) const {
		auto xd = p.x - x;
		auto yd = p.y - y;
		return xd * xd + yd * yd;
	}


	void negate() {
		x = -x;
		y = -y;
	}

	Vec2 negated() const {
		return Vec2(-x, -y);
	}

	Vec2 operator-() const {
		return negated();
	}

};

//specialization for serialization
template<>
struct serialize<Vec2>{
	void static write(const Vec2& val, BinaryStream& stream){
		stream.writeFloat(val.x);
		stream.writeFloat(val.y);
	}

	Vec2 static read(ReadOnlyBinaryStream& stream){
		auto x = stream.getFloat();
		auto y = stream.getFloat();
		return Vec2(x, y);
	}
};
