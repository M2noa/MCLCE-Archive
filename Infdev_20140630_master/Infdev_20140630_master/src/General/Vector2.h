#pragma once

#include "stdafx.h"

#include <string>
#include <glm/glm.hpp>

namespace Esteem
{
	template<typename T, glm::qualifier P>
	struct Vector2t : public glm::vec<2, T, P>
	{
	public:
		static const Vector2t<T, P> zero;
		static const Vector2t<T, P> one;
		
		Vector2t()
			: glm::vec<2, T, P>() {}

		/// \brief conversion constructor
		Vector2t(const struct glm::vec<2, T, P>& v)
			: glm::vec<2, T, P>(v) {}

		Vector2t(T xy)
			: glm::vec<2, T, P>(xy, xy) {}

		Vector2t(T x, T y)
			: glm::vec<2, T, P>(x, y) {}

		// converting constructor
		template <typename U, glm::precision Q>
		explicit Vector2t(const Vector2t<U, Q>& v)
			: glm::vec<2, T, P>(v) {}

		inline void Normalize() { *this = glm::normalize(this); }
		inline const Vector2t<T, P> Normalized() const { return glm::normalize(this); }

		operator glm::vec<2, T, P>&() { return *this; }


		void operator*= (T value) { this->x *= value; this->y *= value; }
		void operator/= (T value) { this->x /= value; this->y /= value; }

		Vector2t<T, P> operator* (T value) { return Vector2t<T, P>(this->x *= value, this->y *= value); }
		Vector2t<T, P> operator/ (T value) { return Vector2t<T, P>(this->x /= value, this->y /= value); }

		inline operator std::string() const { return "Vector2(" + std::to_string(this->x) + ", " + std::to_string(this->y) + ")"; }
	};

	template <typename T, glm::precision P>
	const Vector2t<T, P> Vector2t<T, P>::zero = Vector2t<T, P>(0, 0);
	template <typename T, glm::precision P>
	const Vector2t<T, P> Vector2t<T, P>::one = Vector2t<T, P>(1, 1);

	typedef		Vector2t<float, glm::highp>		Vector2;
	typedef		Vector2							Vector2f;
	typedef		Vector2t<double, glm::highp>	Vector2d;
	typedef		Vector2t<int, glm::highp>		Vector2i;
	typedef		Vector2t<uint, glm::highp>		Vector2u;
}