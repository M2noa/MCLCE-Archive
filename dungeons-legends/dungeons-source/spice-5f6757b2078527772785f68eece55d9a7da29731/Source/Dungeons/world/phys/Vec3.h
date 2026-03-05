/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

//#include <glm/glm.hpp>
#include <string>
#include <algorithm>

//#include "network/BinaryStream.h"

class BlockPos;
class Vec2;
class Pos;

class Vec3 {
public:

	static const Vec3 ZERO, ONE, UNIT_X, NEG_UNIT_X, UNIT_Y, NEG_UNIT_Y, UNIT_Z, NEG_UNIT_Z, MAX, MIN;

	static Vec3 lerp(const Vec3& a, const Vec3& b, float s) {
		return Vec3(a.x + (b.x - a.x) * s, a.y + (b.y - a.y) * s, a.z + (b.z - a.z) * s);
	}

	static Vec3 lerpComponent(const Vec3& a, const Vec3& b, const Vec3& s) {
		return Vec3(a.x + (b.x - a.x) * s.x, a.y + (b.y - a.y) * s.y, a.z + (b.z - a.z) * s.z);
	}

	static Vec3 min(const Vec3& a, const Vec3& b) {
		return Vec3(
			std::min(a.x, b.x),
			std::min(a.y, b.y),
			std::min(a.z, b.z));
	}

	static Vec3 max(const Vec3& a, const Vec3& b) {
		return Vec3(
			std::max(a.x, b.x),
			std::max(a.y, b.y),
			std::max(a.z, b.z));
	}

	static Vec3 mul(const Vec3& a, const Vec3& b) {
		return Vec3(
			a.x * b.x,
			a.y * b.y,
			a.z * b.z);
	}

	static Vec3 div(const Vec3& a, const Vec3& b) {
		return Vec3(
			a.x / b.x,
			a.y / b.y,
			a.z / b.z);
	}

	explicit Vec3(float s) :
		x(s)
		, y(s)
		, z(s){

	}

	Vec3() :
		Vec3(0){
	}

	Vec3(const BlockPos& pos);
	explicit Vec3(const Pos& pos);

	Vec3(float x_, float y_, float z_) :
		x(x_)
		, y(y_)
		, z(z_) {
	}

	Vec3(const FVector& uvec) :
		x(uvec.X)
		, y(uvec.Y)
		, z(uvec.Z) {
	}

	Vec3* set(float x_, float y_, float z_) {
		x = x_;
		y = y_;
		z = z_;
		return this;
	}

	Vec3 operator+(const Vec3& rhs) const {
		return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	Vec3 operator+(float f) const {
		return Vec3(x + f, y + f, z + f);
	}

	Vec3& operator=(const Vec3& rhs) {
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		return *this;
	}

	Vec3& operator+=(const Vec3& rhs) {
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}

	Vec3 operator-(const Vec3& rhs) const {
		return Vec3(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	Vec3 operator-(float f) const {
		return Vec3(x - f, y - f, z - f);
	}

	Vec3& operator-=(const Vec3& rhs) {
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}

	operator FVector() const {
		return FVector(x, y, z);
	}

	Vec3 operator*(float k) const {
		return Vec3(x * k, y * k, z * k);
	}

	Vec3 operator/(float k) const {
		return (*this) * (1.f / k);
	}

	float operator*(const Vec3& v) const {
		return dot(v);
	}

	Vec3& operator*=(float k) {
		x *= k;
		y *= k;
		z *= k;
		return *this;
	}

	bool operator==(const Vec3& rhs) const {
		return x == rhs.x && y == rhs.y && z == rhs.z;
	}

	bool operator<(const Vec3& rhs) const {
		return x < rhs.x && y < rhs.y && z < rhs.z;
	}

	bool operator>(const Vec3& rhs) const {
		return x > rhs.x && y > rhs.y && z > rhs.z;
	}

	bool operator<=(const Vec3& rhs) const {
		return x <= rhs.x && y <= rhs.y && z <= rhs.z;
	}

	bool operator>=(const Vec3& rhs) const {
		return x >= rhs.x && y >= rhs.y && z >= rhs.z;
	}
	
	bool operator!=(const Vec3& rhs)const {
		return !(*this == rhs);
	}

	const float& operator[](int index) const {
		switch(index) {
			case 0: return x;
			case 1: return y;
			case 2: return z;
			// Out of bounds
			default: DEBUG_ASSERT(false, "Invalid index for Vec3, valid is [0,2]");
			return x;
		}
	}

	float& operator[](int index) {
		switch(index) {
			case 0: return x;
			case 1: return y;
			case 2: return z;
			// Out of bounds
			default: DEBUG_ASSERT(false, "Invalid index for Vec3, valid is [0,2]");
			return x;
		}
	}

	explicit operator bool() const {
		return x != 0 || y != 0 || z != 0;
	}

	Vec3 normalized() const {
		const float dist = sqrt(x * x + y * y + z * z);
		return (dist < 0.0001f) ? Vec3::ZERO : Vec3(x / dist, y / dist, z / dist);	// newTemp
	}

	// need this version in certain circumstances when the original length is actually quite small, but the normal will still be valid
	Vec3 normalizedNoTruncate() const {
		const float dist = sqrt(x * x + y * y + z * z);
		return Vec3(x / dist, y / dist, z / dist);	// newTemp
	}

	float dot(const Vec3& p) const {
		return x * p.x + y * p.y + z * p.z;
	}

	Vec3 operator^(const Vec3& p) const {
		return Vec3(y * p.z - z * p.y, z * p.x - x * p.z, x * p.y - y * p.x);	// newTemp
	}

	Vec3 cross(const Vec3& p) const {
		return operator^(p);
	}

	Vec3 add(float x_, float y_, float z_) const {
		return Vec3(x + x_, y + y_, z + z_);	// newTemp
	}

	Vec3 sub(float x_, float y_, float z_) const {
		return Vec3(x - x_, y - y_, z - z_);	// newTemp
	}

	void negate() {
		x = -x;
		y = -y;
		z = -z;
	}

	Vec3 negated() const {
		return Vec3(-x, -y, -z);
	}

	Vec3 operator-() const {
		return negated();
	}

	float distanceTo(const Vec3& p) const {
		const float xd = p.x - x;
		const float yd = p.y - y;
		const float zd = p.z - z;
		return sqrt(xd * xd + yd * yd + zd * zd);
	}

	float distanceToSqr(const Vec3& p) const {
		const float xd = p.x - x;
		const float yd = p.y - y;
		const float zd = p.z - z;
		return xd * xd + yd * yd + zd * zd;
	}

	float distanceToSqr(float x2, float y2, float z2) const {
		const float xd = x2 - x;
		const float yd = y2 - y;
		const float zd = z2 - z;
		return xd * xd + yd * yd + zd * zd;
	}

	float length() const {
		return sqrt(x * x + y * y + z * z);
	}

	float lengthSquared() const {
		return x * x + y * y + z * z;
	}

	float minComponent() const;

	float maxComponent() const;

	bool clipX(const Vec3& b, float xt, Vec3& result) const {
		const float xd = b.x - x;
		const float yd = b.y - y;
		const float zd = b.z - z;

		if (xd * xd < 0.0000001f) {
			return false;
		}

		const float d = (xt - x) / xd;
		if (d < 0 || d > 1) {
			return false;
		}

		result.set(x + xd * d, y + yd * d, z + zd * d);
		return true;
	}

	bool clipY(const Vec3& b, float yt, Vec3& result) const {
		const float xd = b.x - x;
		const float yd = b.y - y;
		const float zd = b.z - z;

		if (yd * yd < 0.0000001f) {
			return false;
		}

		const float d = (yt - y) / yd;
		if (d < 0 || d > 1) {
			return false;
		}

		result.set(x + xd * d, y + yd * d, z + zd * d);
		return true;
	}

	bool clipZ(const Vec3& b, float zt, Vec3& result) const {
		const float xd = b.x - x;
		const float yd = b.y - y;
		const float zd = b.z - z;

		if (zd * zd < 0.0000001f) {
			return false;
		}

		const float d = (zt - z) / zd;
		if (d < 0 || d > 1) {
			return false;
		}

		result.set(x + xd * d, y + yd * d, z + zd * d);
		return true;
	}

	// Moves the supplied vector into the inclusive clip boundary.
	static Vec3 clamp(const Vec3& v, const Vec3& clipMin, const Vec3& clipMax);

	// Moves the supplied vector along the supplied delta normal until it
	// resides within the inclusive clip boundary.
	static bool clampAlongNormal(const Vec3& input, const Vec3& delta, const Vec3& clipMin, const Vec3& clipMax, Vec3& result);

	std::string toString() const;

	void xRot(float degs) {
		const float cosv = cos(degs);
		const float sinv = sin(degs);

		const float xx = x;
		const float yy = y * cosv + z * sinv;
		const float zz = z * cosv - y * sinv;

		x = xx;
		y = yy;
		z = zz;
	}

	void yRot(float degs) {
		const float cosv = cos(degs);
		const float sinv = sin(degs);

		const float xx = x * cosv + z * sinv;
		const float yy = y;
		const float zz = z * cosv - x * sinv;

		x = xx;
		y = yy;
		z = zz;
	}

	void zRot(float degs) {
		const float cosv = cos(degs);
		const float sinv = sin(degs);

		const float xx = x * cosv + y * sinv;
		const float yy = y * cosv - x * sinv;
		const float zz = z;

		x = xx;
		y = yy;
		z = zz;
	}

	Vec3 xz() const;

	Vec3 abs() const {
		return{ std::abs(x), std::abs(y), std::abs(z) };
	}

	Vec3 selectSmallestAxis() const {
		auto absolute = abs();
		if (absolute.x < absolute.y && absolute.x < absolute.z) {
			return{ x, 0, 0 };
		}
		else {
			return absolute.y < absolute.z ? Vec3{0, y, 0} : Vec3{0, 0, z};
		}
	}

	Vec3 floor(float offset = 0.0f) const {
		return Vec3(std::floor(x + offset), std::floor(y + offset), std::floor(z + offset));
	}

	Vec3 ceil() const {
		return Vec3(std::ceil(x), std::ceil(y), std::ceil(z));
	}

	bool isNan() const;

	static Vec3 fromPolarXY(float angle, float radius) {
		return Vec3( radius * cos(angle), radius * sin(angle), 0.f );
	}

	static Vec3 directionFromRotation(Vec2 const& rot);
	static Vec3 directionFromRotation(float rotX, float rotY);
	
	//static Vec2 rotationFromPoints(const Vec3 & from, const Vec3 & to);
	static Vec2 rotationFromDirection(const Vec3 & dir);

	float x, y, z;
};
