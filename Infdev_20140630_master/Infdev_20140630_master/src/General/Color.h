#pragma once

#include <cppu/dtypes.h>
#include <limits>
#include <string>

namespace Esteem
{
	namespace details
	{
		template<typename T, bool = std::is_integral<T>::value>
		struct color_default {};

		template<typename T>
		struct color_default<T, true>
		{
			inline static const T value = std::numeric_limits<T>::max();
		};

		template<typename T>
		struct color_default<T, false>
		{
			inline static const T value = T(1);
		};
	}

	template<class T>
	class Color_t
	{
	public:
		static const Color_t<T> red;
		static const Color_t<T> green;
		static const Color_t<T> blue;

		static const Color_t<T> yellow;
		static const Color_t<T> magenta;
		static const Color_t<T> cyan;

		static const Color_t<T> white;
		static const Color_t<T> black;

		T r;
		T g;
		T b;
		T a;

		Color_t() : r(0), g(0), b(0), a(details::color_default<T>::value) {};
		Color_t(T rgb) : r(rgb), g(rgb), b(rgb), a(details::color_default<T>::value) {};
		Color_t(T r, T g, T b) : r(r), g(g), b(b), a(details::color_default<T>::value) {};
		Color_t(T r, T g, T b, T a) : r(r), g(g), b(b), a(a) {};

		bool operator == (const Color_t<T>& rhs)
		{
			return this->r == rhs.r && this->g == rhs.g && this->b == rhs.b && this->a == rhs.a;
		}

		bool operator != (const Color_t<T>& rhs)
		{
			return this->r != rhs.r || this->g != rhs.g || this->b != rhs.b || this->a != rhs.a;
		}

		T& operator[] (uint i)
		{
			return *(&r + i);
		}

		const T& operator[] (uint i) const
		{
			return *(&r + i);
		}

		Color_t<T>& operator *= (const Color_t<T>& rhs)
		{
			this->r *= rhs.r;
			this->g *= rhs.g;
			this->b *= rhs.b;
			this->a *= rhs.a;
		}

		Color_t<T> operator * (const Color_t<T>& rhs)
		{
			return Color(this->r * rhs.r, this->g * rhs.g, this->b * rhs.b, this->a * rhs.a);
		}

		Color_t<T>& operator /= (const Color_t<T>& rhs)
		{
			this->r /= rhs.r;
			this->g /= rhs.g;
			this->b /= rhs.b;
			this->a /= rhs.a;
		}

		Color_t<T> operator / (const Color_t<T>& rhs)
		{
			return Color(this->r / rhs.r, this->g / rhs.g, this->b / rhs.b, this->a / rhs.a);
		}

		Color_t<T>& operator += (const Color_t<T>& rhs)
		{
			this->r += rhs.r;
			this->g += rhs.g;
			this->b += rhs.b;
			this->a += rhs.a;
		}

		Color_t<T> operator + (const Color_t<T>& rhs)
		{
			return Color(this->r + rhs.r, this->g + rhs.g, this->b + rhs.b, this->a + rhs.a);
		}

		Color_t<T>& operator -= (const Color_t<T>& rhs)
		{
			this->r -= rhs.r;
			this->g -= rhs.g;
			this->b -= rhs.b;
			this->a -= rhs.a;
		}

		Color_t<T> operator - (const Color_t<T>& rhs)
		{
			return Color(this->r - rhs.r, this->g - rhs.g, this->b - rhs.b, this->a - rhs.a);
		}

		inline explicit operator std::string() const { return "Color(" + std::to_string(this->r) + ", " + std::to_string(this->g) + ", " + std::to_string(this->b) + ", " + std::to_string(this->a) + ")"; }
	};

	template<class T>
	const Color_t<T> Color_t<T>::red = Color_t<T>(details::color_default<T>::value, T(0), T(0), details::color_default<T>::value);
	template<class T>
	const Color_t<T> Color_t<T>::green = Color_t<T>(T(0), details::color_default<T>::value, T(0), details::color_default<T>::value);
	template<class T>
	const Color_t<T> Color_t<T>::blue = Color_t<T>(T(0), T(0), details::color_default<T>::value, details::color_default<T>::value);

	template<class T>
	const Color_t<T> Color_t<T>::yellow = Color_t<T>(details::color_default<T>::value, details::color_default<T>::value, T(0), details::color_default<T>::value);
	template<class T>
	const Color_t<T> Color_t<T>::magenta = Color_t<T>(details::color_default<T>::value, T(0), details::color_default<T>::value, details::color_default<T>::value);
	template<class T>
	const Color_t<T> Color_t<T>::cyan = Color_t<T>(T(0), details::color_default<T>::value, details::color_default<T>::value, details::color_default<T>::value);

	template<class T>
	const Color_t<T> Color_t<T>::white = Color_t<T>(details::color_default<T>::value, details::color_default<T>::value, details::color_default<T>::value, details::color_default<T>::value);
	template<class T>
	const Color_t<T> Color_t<T>::black = Color_t<T>(T(0), T(0), T(0), details::color_default<T>::value);

	typedef Color_t<float> Color;
	typedef Color_t<uint8> Color8u;
}