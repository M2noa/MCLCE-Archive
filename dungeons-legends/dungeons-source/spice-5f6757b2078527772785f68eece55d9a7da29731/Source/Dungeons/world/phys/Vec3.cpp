#include "Dungeons.h"

#include "util/Math.h"
#include "world/phys/Vec3.h"
#include "world/phys/Vec2.h"
#include "world/level/BlockPos.h"
#include "world/Pos.h"
#include "util/Math.h"
#include <sstream>
#include <string>

const Vec3 Vec3::ZERO = Vec3();
const Vec3 Vec3::ONE = Vec3(1);
const Vec3 Vec3::UNIT_X = Vec3(1, 0, 0);
const Vec3 Vec3::NEG_UNIT_X = Vec3(-1, 0, 0);
const Vec3 Vec3::UNIT_Y = Vec3(0, 1, 0);
const Vec3 Vec3::NEG_UNIT_Y = Vec3(0, -1, 0);
const Vec3 Vec3::UNIT_Z = Vec3(0, 0, 1);
const Vec3 Vec3::NEG_UNIT_Z = Vec3(0, 0, -1);
const Vec3 Vec3::MAX = Vec3(FLT_MAX);
const Vec3 Vec3::MIN = Vec3(-FLT_MAX);

Vec3::Vec3(const BlockPos& pos) :
	x((float)pos.x)
	, y((float)pos.y)
	, z((float)pos.z) {

}

Vec3::Vec3(const Pos& pos) :
	x((float)pos.x)
	, y((float)pos.y)
	, z((float)pos.z) {

}

float Vec3::minComponent() const {
	return Math::min(Math::min(x, y), z);
}

float Vec3::maxComponent() const {
	return Math::max(Math::max(x, y), z);
}

// Moves the supplied vector into the inclusive clip boundary.
Vec3 Vec3::clamp(const Vec3& v, const Vec3& clipMin, const Vec3& clipMax){
	return Vec3(
		Math::clamp(v.x, clipMin.x, clipMax.x),
		Math::clamp(v.y, clipMin.y, clipMax.y),
		Math::clamp(v.z, clipMin.z, clipMax.z)
		);
}

bool Vec3::isNan() const {
	return Math::isNan(x) ||
			Math::isNan(y) ||
			Math::isNan(z);
}


// Moves the supplied vector along the supplied delta normal until it
// resides within the inclusive clip boundary.
bool Vec3::clampAlongNormal(const Vec3& input, const Vec3& delta, const Vec3& clipMin, const Vec3& clipMax, Vec3& result){
	// Clamp the input to the bounds.  If the input does not change, the
	// input is already inside of the bounds and can be returned.
	const Vec3 inputClamped = clamp(input, clipMin, clipMax);
	if (inputClamped == input) {
		result = input;
		return true;
	}

	// Calculate the per-component error.
	const Vec3 error = inputClamped - input;

	// Divide the error by the delta.  For every component, if it is
	// non-zero for the error and zero or of opposite sign for the delta,
	// the input cannot be clamped.
	if (
		(error.x != 0 && Math::sign(delta.x) != Math::sign(error.x)) ||
		(error.y != 0 && Math::sign(delta.y) != Math::sign(error.y)) ||
		(error.z != 0 && Math::sign(delta.z) != Math::sign(error.z))
		) {
		return false;
	}

	// Advance the input along the delta vector far enough to eliminate the
	// greatest component of error.
	const Vec3 errorOverDelta = Vec3(
		error.x / delta.x,
		error.y / delta.y,
		error.z / delta.z
		);
	const float multiplier = errorOverDelta.maxComponent();
	const Vec3 possibleResult = input + delta * multiplier;

	// Discard the result if it is not inside the boundary.
	const Vec3 clampedResultDelta = possibleResult - clamp(possibleResult, clipMin, clipMax);
	const float clampedResultDeltaMag = clampedResultDelta.length();

	// our worlds can get big, don't want raycasts failing at positions far from the origin, thus the loose tolerance
	if (clampedResultDeltaMag > 0.01f) {
		return false;
	}

	// The input has successfully been cast into the boundary.
	result = possibleResult;
	return true;
}

Vec3 Vec3::directionFromRotation(Vec2 const& rot) {
	return directionFromRotation(rot.x, rot.y);
}

Vec3 Vec3::directionFromRotation(float rotX, float rotY) {

	const float yCos = Math::cos(-rotY * Math::DEGRAD - PI);
	const float ySin = Math::sin(-rotY * Math::DEGRAD - PI);
	const float xCos = -Math::cos(-rotX * Math::DEGRAD);
	const float xSin = Math::sin(-rotX * Math::DEGRAD);

	return Vec3(ySin * xCos, xSin, yCos * xCos);
}
 
std::string Vec3::toString() const {
	std::stringstream ss;
	ss << "Vec3(" << x << "," << y << "," << z << ")";
	return ss.str();
}

Vec3 Vec3::xz() const
{
	return{ x, 0, z };
}

Vec2 Vec3::rotationFromDirection(const Vec3 & dir) {
	const float sd = sqrt(dir.x * dir.x + dir.z * dir.z);

	const float yRotD = (float)(atan2(dir.z, dir.x) * 180 / PI) - 90;
	const float xRotD = (float)-(atan2(dir.y, sd) * 180 / PI);

	return Vec2(xRotD, yRotD);
}