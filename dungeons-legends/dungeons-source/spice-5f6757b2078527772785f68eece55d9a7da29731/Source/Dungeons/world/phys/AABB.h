/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

#include "common_header.h"

#include "world/phys/Vec3.h"
#include "world/phys/HitResult.h"

class AABB {
public:
	static const AABB EMPTY;
	static const AABB BLOCK_SHAPE;

	bool operator==(const AABB& rhs) const;

	static AABB fromIntersection(const AABB& a, const AABB& b) {
		return{
			Vec3::max(a.min, b.min),
			Vec3::min(a.max, b.max)
		};
	}
	
	AABB();
	AABB(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);
	AABB(const Vec3& min, const Vec3& max);
	AABB(const Vec3& min, float side);
	AABB& set(const Vec3& min, const Vec3& max);
	AABB& fromPoints(const Vec3& p0, const Vec3& p1);

	AABB expanded(const Vec3& toContain) const;
	AABB& expand(const Vec3& toContain);
	AABB translated(const Vec3& t) const;
	AABB merge( const AABB& other ) const;
	AABB grow(const Vec3& distance) const;
	AABB cloneMove(const Vec3& distance) const;
	// ya/za is the movement of c this frame. Returns the adjusted velocity to avoid a collision and resolves collision along the given axis if oneway == false
	float clipXCollide(const AABB& c, float ya, bool oneway = false) const;
	float clipYCollide(const AABB& c, float ya, bool oneway = false) const;
	float clipZCollide(const AABB& c, float za, bool oneway = false) const;
	// Same idea as clipX/Y/ZCollide except this does all at once and resolves penetration along the smallest separating axis. This assumes relatively axis-aligned movement
	Vec3 clipCollide(const AABB& c, const Vec3& velocity, bool oneWay = false, float* penetration = nullptr) const;
	bool intersects(const AABB& c) const;
	bool intersectsInner(const AABB& c) const;
	bool intersects(const Vec3& segmentBegin, const Vec3& segmentEnd) const;
	bool intersectSegment(const Vec3& segmentBegin, const Vec3& segmentEnd, Vec3& intersection, Vec3& intersectNorm) const;
	Vec3 getSmallestAxisOut(const AABB& other) const;
	Vec3 axisInside(const AABB& other, Vec3 axis) const;

	float distanceTo(const Vec3& pos) const;
	float distanceToSqr(const Vec3& pos) const;

	float distanceTo(const AABB& aabb) const;
	float distanceToSqr(const AABB& aabb) const;

	AABB& move(float xa, float ya, float za);
	AABB& move(const Vec3& delta);
	AABB& centerAt(const Vec3& center);
	AABB resize(const Vec3& newBounds) const;

	bool contains(const Vec3& p) const;
	float getSize() const;
	bool isNan() const;
	AABB shrink(const Vec3& offset) const;
	AABB flooredCopy(float offsetMin = 0.0f, float offsetMax = 0.0f) const;
	AABB flooredCeiledCopy() const;
	HitResult clip(const Vec3& a, const Vec3& b) const;
	bool isEmpty() const;

	Vec3 getCenter() const;
	Vec3 getBounds() const;

	bool containsX( const Vec3& v ) const {
		return v.y >= min.y && v.y <= max.y && v.z >= min.z && v.z <= max.z;
	}
	bool containsY( const Vec3& v ) const {
		return v.x >= min.x && v.x <= max.x && v.z >= min.z && v.z <= max.z;
	}
	bool containsZ( const Vec3& v ) const {
		return v.x >= min.x && v.x <= max.x && v.y >= min.y && v.y <= max.y;
	}

	void set(const AABB& b);
	void set(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);

	std::string toString() const;

	Vec3 min;
	Vec3 max;

private:
	// Do the work of a clipX/Y/ZCollide. axisIndex determines the index into the Vec3 that we're clipping
	float _clipAxisCollide(const AABB& c, float vAxis, int axisIndex, bool oneway) const;

	bool empty;
};
