#include "Dungeons.h"
#include "Vec2.h"

const Vec2 Vec2::ZERO = Vec2();
const Vec2 Vec2::ONE = Vec2(1.f, 1.f);
const Vec2 Vec2::UNIT_X = Vec2(1.f, 0.f);
const Vec2 Vec2::NEG_UNIT_X = Vec2(-1.f, 0.f);
const Vec2 Vec2::UNIT_Y = Vec2(0.f, 1.f);
const Vec2 Vec2::NEG_UNIT_Y = Vec2(0.f, -1.f);
const Vec2 Vec2::MAX = Vec2(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
const Vec2 Vec2::MIN = Vec2(std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
