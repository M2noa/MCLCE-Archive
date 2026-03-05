#include "pch_core.h"

#include "Color.h"
//#include "Core/Debug/DebugUtils.h"

const Color Color::WHITE = Color(1, 1, 1, 1);
const Color Color::GREY = Color(0.5f, 0.5f, 0.5f, 1.f);
const Color Color::BLACK = Color(0, 0, 0, 1);
const Color Color::RED = Color(1, 0, 0, 1);
const Color Color::GREEN = Color(0, 1, 0, 1);
const Color Color::BLUE = Color(0, 0, 1, 1);
const Color Color::YELLOW = Color(1, 1, 0, 1);
const Color Color::ORANGE = Color(0.85f, 0.5f, 0.2f, 1);
const Color Color::PURPLE = Color(1, 0, 1, 1);
const Color Color::CYAN = Color(0, 1, 1, 1);
const Color Color::NIL = Color(0, 0, 0, 0);
const Color Color::SHADE_DOWN = Color::fromIntensity(0.5f);
const Color Color::SHADE_UP = Color::fromIntensity(1.0f);
const Color Color::SHADE_NORTH_SOUTH = Color::fromIntensity(0.8f);
const Color Color::SHADE_WEST_EAST = Color::fromIntensity(0.6f);

Color Color::fromHSB(float hue, float saturation, float brightness) {

	if (saturation == 0) {
		return Color(brightness, brightness, brightness);
	}

	float h = (hue - (float)floorf(hue)) * 6.0f;
	float f = h - (float)floorf(h);
	float p = brightness * (1.0f - saturation);
	float q = brightness * (1.0f - saturation * f);
	float t = brightness * (1.0f - (saturation * (1.0f - f)));

	switch ((int)h) {
		case 0:
			return Color(brightness, t, p);
		case 1:
			return Color(q, brightness, p);
		case 2:
			return Color(p, brightness, t);
		case 3:
			return Color(p, q, brightness);
		case 4:
			return Color(t, p, brightness);
		case 5:
			return Color(brightness, p, q);
		default:
			return Color::NIL;
	}
}

int Color::toARGB() const {
	DEBUG_ASSERT(isNormalized(), "This color uses more than 8 bit per channel, clamp it first");
	unsigned int red = static_cast<int>(r * 255);
	unsigned int green = static_cast<int>(g * 255);
	unsigned int blue = static_cast<int>(b * 255);
	unsigned int alpha = static_cast<int>(a * 255);
	int p = alpha << 24;
	p += red << 16;
	p += green << 8;
	p += blue;
	return p;
}

int Color::toABGR() const {
	DEBUG_ASSERT(isNormalized(), "This color uses more than 8 bit per channel, clamp it first");
	unsigned int red = static_cast<int>(r * 255);
	unsigned int green = static_cast<int>(g * 255);
	unsigned int blue = static_cast<int>(b * 255);
	unsigned int alpha = static_cast<int>(a * 255);
	int p = alpha << 24;
	p += blue << 16;
	p += green << 8;
	p += red;
	return p;
}
