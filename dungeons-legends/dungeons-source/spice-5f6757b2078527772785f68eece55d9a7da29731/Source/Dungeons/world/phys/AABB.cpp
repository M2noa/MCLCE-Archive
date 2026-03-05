/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"
#include "world/phys/AABB.h"
#include "CommonTypes.h"

const AABB AABB::EMPTY(Vec3::ZERO, Vec3::ZERO);

AABB::AABB() :
	min(0)
	, max(1) {
	empty = false;
}

AABB::AABB(float minX, float minY, float minZ,
	float maxX, float maxY, float maxZ)
	: min(minX, minY, minZ)
	, max(maxX, maxY, maxZ) {
	DEBUG_ASSERT(min.x <= max.x, "min.x must always be <= max.x");
	DEBUG_ASSERT(min.y <= max.y, "min.y must always be <= max.y");
	DEBUG_ASSERT(min.z <= max.z, "min.z must always be <= max.z");

	empty = min == Vec3::ZERO
		&& max == Vec3::ZERO;
}

AABB::AABB(const Vec3& min, const Vec3& max) :
	min(min)
	, max(max) {
	DEBUG_ASSERT(min.x <= max.x, "min.x must always be <= max.x");
	DEBUG_ASSERT(min.y <= max.y, "min.y must always be <= max.y");
	DEBUG_ASSERT(min.z <= max.z, "min.z must always be <= max.z");
	empty = min == Vec3::ZERO
		&& max == Vec3::ZERO;
}

AABB::AABB(const Vec3& min, float side) :
	AABB(min, min + Vec3(side, side, side)) {
}

AABB& AABB::set(const Vec3& min, const Vec3& max) {
	this->min = min;
	this->max = max;
	DEBUG_ASSERT(min.x <= max.x, "min.x must always be <= max.x");
	DEBUG_ASSERT(min.y <= max.y, "min.y must always be <= max.y");
	DEBUG_ASSERT(min.z <= max.z, "min.z must always be <= max.z");
	empty = min == Vec3::ZERO
		&& max == Vec3::ZERO;
	return *this;
}

AABB& AABB::fromPoints(const Vec3& p0, const Vec3& p1) {
	min.x = std::min(p0.x, p1.x);
	min.y = std::min(p0.y, p1.y);
	min.z = std::min(p0.z, p1.z);

	max.x = std::max(p0.x, p1.x);
	max.y = std::max(p0.y, p1.y);
	max.z = std::max(p0.z, p1.z);
	
	return *this;
}

AABB& AABB::expand(const Vec3& toContain) {
	min = Vec3::min(min, toContain);
	max = Vec3::max(max, toContain);
	return *this;
}

AABB AABB::expanded(const Vec3& toContain) const {
	AABB res(min, max);

	if(toContain.x < 0) {
		res.min.x += toContain.x;
	}
	else if(toContain.x > 0) {
		res.max.x += toContain.x;
	}

	if(toContain.y < 0) {
		res.min.y += toContain.y;
	}
	else if(toContain.y > 0) {
		res.max.y += toContain.y;
	}

	if(toContain.z < 0) {
		res.min.z += toContain.z;
	}
	else if(toContain.z > 0) {
		res.max.z += toContain.z;
	}

	res.empty = min == Vec3::ZERO
		&& max == Vec3::ZERO;

	return res;
}

AABB AABB::translated(const Vec3& t) const {
	return AABB(min + t, max + t);
}

AABB AABB::merge(const AABB& other) const {
	return AABB(
		Vec3(std::min(min.x, other.min.x),
		std::min(min.y, other.min.y),
		std::min(min.z, other.min.z)),
		Vec3(std::max(max.x, other.max.x),
		std::max(max.y, other.max.y),
		std::max(max.z, other.max.z)));
}

AABB AABB::grow(const Vec3& distance) const {
	return AABB(min - distance, max + distance);
}

AABB AABB::cloneMove(const Vec3& distance) const {
	return AABB(min + distance, max + distance);
}

Vec3 AABB::clipCollide(const AABB& c, const Vec3& velocity, bool oneWay, float* penetration) const
{
	if(penetration) {
		*penetration = 0.0f;
	}

	Vec3 resolvedVelocity = velocity;
	// Positive penetration values on each cardinal axis
	float axisPenetrations[3];
	// Signed penetration values where negative values indicate separation. Could use only this and not normals, but it's conceptually simpler this way
	float axisPenetrationsSigned[3];
	// Usually this would be a vector, but since we always resolve along cardinal axes, this just indicates the sign (-1, or 1)
	float normalDirs[3];
	// Number of separating axes
	int separatingAxes = 0;
	// Index of separating axis
	int separatingAxis = 0;
	float resultPenetration = std::numeric_limits<float>::max();

	// Determine separating and non-separating axes and store information for sweep test
	for(int i = 0; i < 3; ++i) {
		// Penetration on min and max side, NOT a minimum and maximum amount of penetration
		float minPenetration = c.max[i] - min[i];
		float maxPenetration = max[i] - c.min[i];

		// Clip off negative values, because that indicates separation, which is fine
		float minPositive = std::max(0.0f, minPenetration);
		float maxPositive = std::max(0.0f, maxPenetration);

		// If there's separation on one side that's good enough for the axis, because the other one will be deep penetration
		// If there is separation, store the signed value for later use during sweeping
		// Minimum side is separating side
		if(minPositive == 0.0f) {
			axisPenetrations[i] = 0.0f;
			axisPenetrationsSigned[i] = minPenetration;
			normalDirs[i] = -1.0f;
			++separatingAxes;
			separatingAxis = i;
		}
		// Maximum side is separating side
		else if(maxPositive == 0.0f) {
			axisPenetrations[i] = 0.0f;
			axisPenetrationsSigned[i] = maxPenetration;
			normalDirs[i] = 1.0f;
			++separatingAxes;
			separatingAxis = i;
		}
		// Penetration on both sides, use the smaller one, in these cases the signed values match the regular ones since they were positive
		else if(minPositive < maxPositive) {
			axisPenetrations[i] = axisPenetrationsSigned[i] = minPositive;
			normalDirs[i] = -1.0f;
		}
		else {
			axisPenetrations[i] = axisPenetrationsSigned[i] = maxPositive;
			normalDirs[i] = 1.0f;
		}

		// We can exit if there's more than one separating axis because our movement is only one-dimensional, so we couldn't collide with this
		if(separatingAxes > 1) {
			return resolvedVelocity;
		}
		resultPenetration = std::min(resultPenetration, axisPenetrations[i]);
	}

	if(penetration) {
		*penetration = resultPenetration;
	}

	// No separating axes means we have a collision
	if(!separatingAxes) {
		// One way mode doesn't resolve penetration, so exit
		if(oneWay) {
			return resolvedVelocity;
		}

		// Find minimum axis of penetration and resolve that way
		int bestAxis = 0;
		// Start at 1 because we just did 0 above
		for(int i = 1; i < 3; ++i) {
			if(axisPenetrations[i] < axisPenetrations[bestAxis]) {
				bestAxis = i;
			}
		}

		// Correct the velocity to resolve penetration on the smallest axis
		resolvedVelocity[bestAxis] = axisPenetrations[bestAxis]*normalDirs[bestAxis];
		return resolvedVelocity;
	}

	// We have 1 separating axis, which means no collision, but we still need to make sure the velocity won't cause one on this axis
	float sweptPenetration = axisPenetrationsSigned[separatingAxis] - normalDirs[separatingAxis]*velocity[separatingAxis];
	// If the swept penetration isn't colliding on this axis, that means it's a separating axis, so there's no swept collision
	if(sweptPenetration <= 0.0f) {
		return resolvedVelocity;
	}

	// Set the velocity on this axis to the separation, in the same direction as it was already going
	// If this happens on multiple axes, we'll end up pointing the velocity into the box's edge (2) or corner (3)
	resolvedVelocity[separatingAxis] = axisPenetrationsSigned[separatingAxis]*normalDirs[separatingAxis];

	// If we got here there was no direct collision but there was a swept collision that we adjusted the velocity for
	return resolvedVelocity;
}

float AABB::_clipAxisCollide(const AABB& c, float vAxis, int axisIndex, bool oneway) const {
	// Get the other indices that we aren't clipping to see if we can early out
	int otherIndexA = (axisIndex + 1) % 3;
	int otherIndexB = (axisIndex + 2) % 3;

	// Early outs for non-collision on the non-primary axes
	// Being exactly on the boundary is not considered a collision, hence <= instead of <
	if(c.max[otherIndexA] <= min[otherIndexA] || c.min[otherIndexA] >= max[otherIndexA] ||
		c.max[otherIndexB] <= min[otherIndexB] || c.min[otherIndexB] >= max[otherIndexB]) {
		return vAxis;
	}

	// Amount and direction we would resolve collision with the top
	float topImpulse = max[axisIndex] - c.min[axisIndex];
	// Same as above but for the swept position
	float topImpulseSwept = topImpulse - vAxis;
	// Positive values resolve collisions with top, so negative is no collision
	if(topImpulse <= 0.0f) {
		// No previous collision, make sure there's also no swept collision
		if(topImpulseSwept <= 0.0f) {
			return vAxis;
		}
		// Sign changed, so this is a swept collision, clamp velocity down to separation to prevent collision
		return topImpulse;
	}

	// Same idea as with top, except the sign is flipped
	float bottomImpulse = min[axisIndex] - c.max[axisIndex];
	float bottomImpulseSwept = bottomImpulse - vAxis;
	// Negative values resolve collisions with bottom, so positive is no collision
	if(bottomImpulse >= 0.0f) {
		if(bottomImpulseSwept >= 0.0f) {
			return vAxis;
		}
		return bottomImpulse;
	}

	// oneway doesn't solve penetration, and swept collisions were handled above
	if(oneway)
		return vAxis;

	// Swept collision and no collision was handled, resolve penetration. Determine lesser overlap and resolve that way
	// May be slightly less branching to expand this out into the below checks, but much harder to read
	// max and min out separation, as we only want to compare penetration
	topImpulse = std::max(0.0f, topImpulse);
	bottomImpulse = std::min(0.0f, bottomImpulse);

	// We know topImpulse is positive or 0, and bottomImpulse is negative or 0
	// If topImpulse is the better direction to resolve in
	if(topImpulse < -bottomImpulse) {
		return topImpulse;
	}

	// bottomImpulse is the better direction to resolve in
	return bottomImpulse;
}

float AABB::clipXCollide(const AABB& c, float ya, bool oneway) const {
	return _clipAxisCollide(c, ya, 0, oneway);
}
float AABB::clipYCollide(const AABB& c, float ya, bool oneway) const {
	return _clipAxisCollide(c, ya, 1, oneway);
}
float AABB::clipZCollide(const AABB& c, float ya, bool oneway) const {
	return _clipAxisCollide(c, ya, 2, oneway);
}

bool AABB::intersects(const AABB& c) const {
	if(c.max.x <= min.x || c.min.x >= max.x) {
		return false;
	}
	if(c.max.y <= min.y || c.min.y >= max.y) {
		return false;
	}
	if(c.max.z <= min.z || c.min.z >= max.z) {
		return false;
	}
	return true;
}

bool AABB::intersectsInner(const AABB& c) const {
	if(c.max.x < min.x || c.min.x > max.x) {
		return false;
	}
	if(c.max.y < min.y || c.min.y > max.y) {
		return false;
	}
	if(c.max.z < min.z || c.min.z > max.z) {
		return false;
	}
	return true;
}

Vec3 AABB::getSmallestAxisOut(const AABB& other) const {
	if(!intersects(other)) {
		return Vec3::ZERO;
	}

	Vec3 inside = other.max - min;
	Vec3 totalDistance = other.min - max;

	// get the shortest axis "out"
	Vec3 smallInside = Vec3::ZERO;
	if(	fabsf(inside.x) < fabsf(inside.y) && fabsf(inside.x) < fabsf(inside.z)) {
		smallInside = Vec3(inside.x, 0.0f, 0.0f);
	} 
	else if(fabsf(inside.z) < fabsf(inside.x) && fabsf(inside.z) < fabsf(inside.y)) {
		smallInside = Vec3(0.0f, 0.0f, inside.z);
	} 
	else {
		smallInside = Vec3(0.0f, inside.y, 0.0f);
	}

	// 
	Vec3 smallOutside = Vec3::ZERO;
	if(fabsf(totalDistance.x) < fabsf(totalDistance.y) && fabsf(totalDistance.x) < fabsf(totalDistance.z)) {
		smallOutside = Vec3(totalDistance.x, 0.0f, 0.0f);
	}
	else if(fabsf(totalDistance.z) < fabsf(totalDistance.x) && fabsf(totalDistance.z) < fabsf(totalDistance.y)) {
		smallOutside = Vec3(0.0f, 0.0f, totalDistance.z);
	}
	else {
		smallOutside = Vec3(0.0f, totalDistance.y, 0.0f);
	}

	// get shortest juan
	if(smallInside.lengthSquared() < smallOutside.lengthSquared()) {
		return smallInside;
	}

	return smallOutside;
}

Vec3 AABB::axisInside(const AABB& other, Vec3 axis) const {
	if(axis.x > 0.0f) {
		return Vec3(max.x - other.min.x, 0.0f, 0.0f);
	}
	else if(axis.x < 0.0f) {
		return Vec3(min.x - other.max.x, 0.0f, 0.0f);
	}
	else if(axis.y > 0.0f) {
		return Vec3(0.0f, max.y - other.min.y, 0.0f);
	}
	else if(axis.y < 0.0f) {
		return Vec3(0.0f, min.y - other.max.y, 0.0f);
	}
	else if(axis.z > 0.0f) {
		return Vec3(0.0f, 0.0f, max.z - other.min.z);
	}
	else if(axis.z < 0.0f) {
		return Vec3(0.0f, 0.0f, min.z - other.max.z);
	}

	return Vec3(0.0f, 0.0f, 0.0f);
}

float AABB::distanceTo(const Vec3& pos) const {
	return Math::sqrt(distanceToSqr(pos));
}

float AABB::distanceToSqr(const Vec3& pos) const {
	Vec3 aabbClosest = Vec3::clamp(pos, min, max);
	return aabbClosest.distanceToSqr(pos);
}

float AABB::distanceTo(const AABB& aabb) const {
	return Math::sqrt(distanceToSqr(aabb));
}

float AABB::distanceToSqr(const AABB& aabb) const {
	float sqrtDist = 0;

	for (int i = 0; i < 3; i++) {
		if (aabb.max[i] < min[i]) {
			float value = aabb.max[i] - min[i];
			sqrtDist += value * value;
		}
		else if (aabb.min[i] > max[i]) {
			float value = aabb.min[i] - max[i];
			sqrtDist += value * value;
		}
	}

	return sqrtDist;
}

bool AABB::intersects(const Vec3& segmentBegin, const Vec3& segmentEnd) const {
	Vec3 d = (segmentEnd - segmentBegin) * 0.5f;
	Vec3 e = (max - min) * 0.5f;
	Vec3 c = segmentBegin + d - (min + max) * 0.5f;
	Vec3 ad(std::abs(d.x), std::abs(d.y), std::abs(d.z));

	if (std::abs(c.x) > e.x + ad.x)
		return false;

	if (std::abs(c.y) > e.y + ad.y)
		return false;

	if (std::abs(c.z) > e.z + ad.z)
		return false;

	static float epsilon = 0.0001f;

	if (std::abs(d.y * c.z - d.z * c.y) > e.y * ad.z + e.z * ad.y + epsilon)
		return false;

	if (std::abs(d.z * c.x - d.x * c.z) > e.z * ad.x + e.x * ad.z + epsilon)
		return false;

	if (std::abs(d.x * c.y - d.y * c.x) > e.x * ad.y + e.y * ad.x + epsilon)
		return false;

	return true;
}

static bool _segmentIntersect(const float boxStart, const float boxEnd, const float segStart, const float segEnd, float& tIn, float& tOut) {
	const float dir = segEnd - segStart;

	//reject parallel cases
	if (fabs(dir) < 0.000001f) {
		if ((segStart < boxStart) || (segStart > boxEnd)) {
			return false;
		}
		else {
			return true; //parallel, but inside
		}
	}

	//order entry and exit values
	float ent = (boxStart - segStart) / dir;
	float ex = (boxEnd - segStart) / dir;
	if (ent > ex) {
		const float tmp = ent;
		ent = ex;
		ex = tmp;
	}

	if ((tIn > ex) || (ent > tOut)) {
		return false;
	}
	else {
		tIn = std::max(tIn, ent);
		tOut = std::min(tOut, ex);
		return true;
	}
}

bool AABB::intersectSegment(const Vec3& segmentBegin, const Vec3& segmentEnd, Vec3& intersectPoint, Vec3& intersectNorm) const {
	// initialize to the segment's boundaries. 
	float tIn = 0.0f;
	float tOut = 1.0f;

	if (!_segmentIntersect(min.x, max.x, segmentBegin.x, segmentEnd.x, tIn, tOut) ||
		!_segmentIntersect(min.y, max.y, segmentBegin.y, segmentEnd.y, tIn, tOut) ||
		!_segmentIntersect(min.z, max.z, segmentBegin.z, segmentEnd.z, tIn, tOut)) {
		return false;
	}
	
	//successful intersection.
	intersectPoint = Vec3::lerp(segmentBegin, segmentEnd, tIn);

	const bool xPos = (segmentEnd.x - segmentBegin.x) > 0.0f;
	const bool yPos = (segmentEnd.y - segmentBegin.y) > 0.0f;
	const bool zPos = (segmentEnd.z - segmentBegin.z) > 0.0f;
	if ((std::abs(intersectPoint.x - min.x) < 0.000001f) || 
		(std::abs(intersectPoint.x - max.x) < 0.000001f)) {
		intersectNorm = Vec3(xPos ? -1.0f : 1.0f, 0.0f, 0.0f);
	}
	else if ((std::abs(intersectPoint.y - min.y) < 0.000001f) ||
			 (std::abs(intersectPoint.y - max.y) < 0.000001f)) {
		intersectNorm = Vec3(0.0f, yPos ? -1.0f : 1.0f, 0.0f);
	}
	else if ((std::abs(intersectPoint.z - min.z) < 0.000001f) ||
			 (std::abs(intersectPoint.z - max.z) < 0.000001f)) {
		intersectNorm = Vec3(0.0f, 0.0f, zPos ? -1.0f : 1.0f);
	}

	return true;
}


AABB& AABB::move(const Vec3& pos) {
	min += pos;
	max += pos;
	return *this;
}

AABB& AABB::move(float xa, float ya, float za) {
	min.x += xa;
	min.y += ya;
	min.z += za;
	max.x += xa;
	max.y += ya;
	max.z += za;
	return *this;
}

AABB& AABB::centerAt(const Vec3& center) {
	auto hb = getBounds() * 0.5f;
	min = center - hb;
	max = center + hb;
	return *this;
}

AABB AABB::resize(const Vec3& newBounds) const {
	auto center = getCenter();
	auto hs = newBounds * 0.5f;
	return{
		center - hs,
		center + hs
	};
}

bool AABB::contains(const Vec3& p) const {
	if(p.x <= min.x || p.x >= max.x) {
		return false;
	}
	if(p.y <= min.y || p.y >= max.y) {
		return false;
	}
	if(p.z <= min.z || p.z >= max.z) {
		return false;
	}
	return true;
}

float AABB::getSize() const {
	const float xs = max.x - min.x;
	const float ys = max.y - min.y;
	const float zs = max.z - min.z;
	return (xs + ys + zs) / 3.0f;
}

AABB AABB::shrink(const Vec3& offset) const {
	Vec3 newMin = min + offset;
	Vec3 newMax = max - offset;

	if (newMin.x > newMax.x) {
		newMin.x = newMax.x = (min.x + max.x) / 2;
	}

	if (newMin.y > newMax.y) {
		newMin.y = newMax.y = (min.y + max.y) / 2;
	}

	if (newMin.z > newMax.z) {
		newMin.z = newMax.z = (min.z + max.z) / 2;
	}

	return AABB(newMin, newMax);
}

HitResult AABB::clip(const Vec3& a, const Vec3& b) const {
	if(isEmpty()) {
		return HitResult(b);
	}
	Vec3 xh0, xh1, yh0, yh1, zh0, zh1;

	bool bxh0 = a.clipX(b, min.x, xh0);
	bool bxh1 = a.clipX(b, max.x, xh1);

	bool byh0 = a.clipY(b, min.y, yh0);
	bool byh1 = a.clipY(b, max.y, yh1);

	bool bzh0 = a.clipZ(b, min.z, zh0);
	bool bzh1 = a.clipZ(b, max.z, zh1);

	//if (!containsX(xh0)) xh0 = nullptr;
	if(!bxh0 || !containsX(xh0)) {
		bxh0 = false;
	}
	if(!bxh1 || !containsX(xh1)) {
		bxh1 = false;
	}
	if(!byh0 || !containsY(yh0)) {
		byh0 = false;
	}
	if(!byh1 || !containsY(yh1)) {
		byh1 = false;
	}
	if(!bzh0 || !containsZ(zh0)) {
		bzh0 = false;
	}
	if(!bzh1 || !containsZ(zh1)) {
		bzh1 = false;
	}

	Vec3* closest = nullptr;

	//if (xh0 != nullptr && (closest == nullptr || a.distanceToSqr(xh0) < a.distanceToSqr(closest))) closest = xh0;
	if(bxh0 && (closest == nullptr || a.distanceToSqr(xh0) < a.distanceToSqr(*closest))) {
		closest = &xh0;
	}
	if(bxh1 && (closest == nullptr || a.distanceToSqr(xh1) < a.distanceToSqr(*closest))) {
		closest = &xh1;
	}
	if(byh0 && (closest == nullptr || a.distanceToSqr(yh0) < a.distanceToSqr(*closest))) {
		closest = &yh0;
	}
	if(byh1 && (closest == nullptr || a.distanceToSqr(yh1) < a.distanceToSqr(*closest))) {
		closest = &yh1;
	}
	if(bzh0 && (closest == nullptr || a.distanceToSqr(zh0) < a.distanceToSqr(*closest))) {
		closest = &zh0;
	}
	if(bzh1 && (closest == nullptr || a.distanceToSqr(zh1) < a.distanceToSqr(*closest))) {
		closest = &zh1;
	}

	if(closest == nullptr) {
		return HitResult(b);
	}

	FacingID face = -1;

	if(closest == &xh0) {
		face = 4;
	}
	if(closest == &xh1) {
		face = 5;
	}
	if(closest == &yh0) {
		face = 0;
	}
	if(closest == &yh1) {
		face = 1;
	}
	if(closest == &zh0) {
		face = 2;
	}
	if(closest == &zh1) {
		face = 3;
	}

	return HitResult(BlockPos(0,0,0), face, *closest);
}

void AABB::set(const AABB& b) {
	min = b.min;
	max = b.max;
	empty = min.x == 0 && min.y == 0 && min.z == 0
		&& max.x == 0 && max.y == 0 && max.z == 0;
}

void AABB::set(float minX, float minY, float minZ, float maxX, float maxY, float maxZ) {
	min = Vec3(minX, minY, minZ);
	max = Vec3(maxX, maxY, maxZ);
	empty = min.x == 0 && min.y == 0 && min.z == 0
		&& max.x == 0 && max.y == 0 && max.z == 0;
}

std::string AABB::toString() const {
	std::stringstream ss;
	ss << "AABB(" << min.x << "," << min.y << "," << min.z << " to " << max.x << "," << max.y << "," << max.z << ")";
	return ss.str();
}

bool AABB::isEmpty() const {
	return empty;
}

AABB AABB::flooredCopy( float offsetMin, float offsetMax) const {
	return AABB(min.floor(offsetMin), max.floor(offsetMax));
}

AABB AABB::flooredCeiledCopy() const {
	return AABB(min.floor(), max.ceil());
}

Vec3 AABB::getCenter() const
{
	return min + ((max - min) * 0.5f);
}

Vec3 AABB::getBounds() const {
	return max - min;
}

bool AABB::isNan() const {
	return max.isNan() || min.isNan();
}

bool AABB::operator==(const AABB& rhs) const {
	return min == rhs.min && max == rhs.max;
}
