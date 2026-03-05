#pragma once

#include "Math.h"

class Color {
public:

	static const Color WHITE, GREY, BLACK, RED, GREEN, BLUE, YELLOW, ORANGE, PURPLE, CYAN, NIL, SHADE_DOWN, SHADE_UP, SHADE_NORTH_SOUTH, SHADE_WEST_EAST;


	static Color lerp(const Color& a, const Color& b, float s) {
		float invs = 1.f - s;
		return Color(
			a.r * invs + b.r * s,
			a.g * invs + b.g * s,
			a.b * invs + b.b * s,
			a.a * invs + b.a * s);
	}

	// lerp a to b and c to d by x, 
	// then lerp from ab to cd by y
	static Color bilinear(const Color& a, const Color& b, const Color& c, const Color& d, float x, float y) {
		return lerp(lerp(a, b, x), lerp(c, d, x), y);
	}
	static Color bilinear(const std::array<Color, 4>& colors, float x, float y) {
		return bilinear(colors[0], colors[1], colors[2], colors[3], x, y);
	}

	///create a color4 from Hue, Saturation and Brightness. Grabbed from OpenJDK 7
	static Color fromHSB(float hue, float saturation, float brightness);

	template<typename T>
	static Color from255Range(T r, T g, T b, T a = T(255)) {
		return{
			static_cast<float>(r) / 255.f,
			static_cast<float>(g) / 255.f,
			static_cast<float>(b) / 255.f,
			static_cast<float>(a) / 255.f,
		};
	}

	static Color fromABGR(int col) {
		return from255Range(
			((col) & 0xff),
			((col >> 8) & 0xff),
			((col >> 16) & 0xff),
			((col >> 24) & 0xff));
	}

	static Color fromARGB(int col) {
		return from255Range(
			((col >> 16) & 0xff),
			((col >> 8) & 0xff),
			((col) & 0xff),
			((col >> 24) & 0xff));
	}

	static Color fromRGB(int col) {
		return from255Range(
			((col >> 16) & 0xff),
			((col >> 8) & 0xff),
			((col) & 0xff),
			0xff);
	}

	static Color fromBytes(const unsigned char* ptr) {
		return from255Range(ptr[0], ptr[1], ptr[2], ptr[3]);
	}

	static Color fromIntensity(float I, float a = 1.f) {
		return{ I,I,I, a };
	}

	float r, g, b, a;

	Color()
		: r(0)
		, g(0)
		, b(0)
		, a(0) {
	}

	Color(float r_, float g_, float b_, float a_ = 1)
		: r(r_)
		, g(g_)
		, b(b_)
		, a(a_) {
	}

	Color(const Color& color, float a_)
		: r(color.r)
		, g(color.g)
		, b(color.b)
		, a(a_) {
	}

	void add(float val, bool modifyAlpha) {
		r += val;
		g += val;
		b += val;
		if (modifyAlpha) {
			a += val;
		}
		clamp();
	}

	void mul(float val, bool modifyAlpha) {
		r *= val;
		g *= val;
		b *= val;
		if (modifyAlpha) {
			a *= val;
		}
		clamp();
	}

	const float* data() const {
		return (float*) this;
	}

	bool isNormalized() const {
		return
			r >= 0.f && r <= 1.f &&
			g >= 0.f && g <= 1.f &&
			b >= 0.f && b <= 1.f &&
			a >= 0.f && a <= 1.f;
	}

	int toARGB() const;

	int toABGR() const;

	float average() const {
		return (r + g + b) * 0.33333333f;
	}

	float luminance() const {
		return r * 0.2126f + g * 0.7152f + b * 0.0722f;
	}

	///access this object just like an array of floats
	inline float& operator [](int idx) {
		return ((float*) this)[idx];
	}

	//multiplies all the members except for alpha, because usually we don't want that
	inline Color& operator*=(float s) {

		r *= s;
		g *= s;
		b *= s;

		return *this;
	}

	inline Color& operator-=(const Color& c) {
		r -= c.r;
		g -= c.g;
		b -= c.b;
		a -= c.a;
		return *this;
	}

	inline Color& operator+=(const Color& c) {
		r += c.r;
		g += c.g;
		b += c.b;
		a += c.a;
		return *this;
	}

	inline Color& operator-=(const float c) {
		r -= c;
		g -= c;
		b -= c;
		a -= c;
		return *this;
	}

	inline bool operator==(const Color& c) const {
		//This float comparison is intended
		return c.r == r && c.g == g && c.b == b && c.a == a;
	}

	inline bool operator!=(const Color& c) const {
		return !(*this == c);
	}

	inline bool operator<(const Color& c) const {
		return std::make_tuple(r, g, b, a) < std::make_tuple(c.r, c.g, c.b, c.a);
	}

	inline Color operator*(float s) const {
		return Color(r * s, g * s, b * s, a * s);
	}

	inline Color operator/(float s) const {
		return Color(r / s, g / s, b / s, a / s);	//yes, / divides alpha and * doesn't, sorry
	}

	Color operator*(const Color& c) const {
		return Color(
			r * c.r,
			g * c.g,
			b * c.b,
			a * c.a);
	}

	inline Color darker(float s) const {
		return{
			std::max(0.f, r - s),
			std::max(0.f, g - s),
			std::max(0.f, b - s),
			a
		};
	}

	inline Color brighter(float s) const {
		return Color(r + s, g + s, b + s, a);
	}

	inline Color operator+(const Color& rhs) const {
		return Color(r + rhs.r, g + rhs.g, b + rhs.b, a + rhs.a);
	}

	inline Color operator-(const Color& rhs) const {
		return Color(r - rhs.r, g - rhs.g, b - rhs.b, a - rhs.a);
	}

	float clamp(float x, float min, float max) {
		return x < min ? min : x < max ? x : max;
	}

	void clamp() {
		r = clamp(r, 0.f, 1.f);
		g = clamp(g, 0.f, 1.f);
		b = clamp(b, 0.f, 1.f);
		a = clamp(a, 0.f, 1.f);
	}

	inline Color clamped() const {
		Color me = *this;
		me.clamp();
		return me;
	}

	inline bool isNil() const {
		return r == 0 && g == 0 && b == 0 && a == 0;
	}

	explicit operator bool() const {
		return r != 0 || g != 0 || b != 0 || a != 0;
	}

	float distanceFrom(const Color& col) const {
		return std::abs(col.r - r) + std::abs(col.g - g) + std::abs(col.b - b);
	}

private:

};

