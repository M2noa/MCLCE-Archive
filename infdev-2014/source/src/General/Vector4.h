#pragma once

#include "stdafx.h"
#include <string>
#include <glm/glm.hpp>

namespace Esteem
{
	template<typename T, glm::qualifier P>
	struct Vector4t : public glm::vec<4, T, P>
	{
	public:
		static const Vector4t<T, P> zero;
		static const Vector4t<T, P> one;

		Vector4t<T, P>()
			: glm::vec<4, T, P>() {}

		/// \brief conversion constructor
		Vector4t<T, P>(const struct glm::vec<4, T, P>& v)
			: glm::vec<4, T, P>(v) {}

		Vector4t<T, P>(T xyz, T w)
			: glm::vec<4, T, P>(xyz, xyz, xyz, w) {}

		Vector4t<T, P>(T x, T y, T z, T w)
			: glm::vec<4, T, P>(x, y, z, w) {}

		// converting constructor
		template <typename U, glm::qualifier Q>
		explicit Vector4t<T, P>(const Vector4t<U, Q>& v)
			: glm::vec<4, T, P>(v) {}

		inline void Normalize() { *this = glm::normalize(this); }
		inline const Vector4t<T, P> Normalized() const { return glm::normalize(this); }

		operator glm::vec<4, T, P>&() { return *this; }


		void operator*= (T value) { this->x *= value; this->y *= value; this->z *= value; this->w *= value; }
		void operator/= (T value) { this->x /= value; this->y /= value; this->z /= value; this->w *= value; }

		Vector4t<T, P> operator* (T value) { return Vector4t<T, P>(this->x *= value, this->y *= value, this->z *= value, this->w *= value); }
		Vector4t<T, P> operator/ (T value) { return Vector4t<T, P>(this->x /= value, this->y /= value, this->z *= value, this->w /= value); }

		inline operator std::string() const { return "Vector4(" + std::to_string(this->x) + ", " + std::to_string(this->y) + ", " + std::to_string(this->z) + ", " + std::to_string(this->w) + ")"; }
	};

	template <typename T, glm::qualifier P>
	const Vector4t<T, P> Vector4t<T, P>::zero = Vector4t<T, P>(0, 0, 0, 0);
	template <typename T, glm::qualifier P>
	const Vector4t<T, P> Vector4t<T, P>::one = Vector4t<T, P>(1, 1, 1, 1);

	typedef		Vector4t<float, glm::highp>		Vector4;
	typedef		Vector4							Vector4f;
	typedef		Vector4t<double, glm::highp>	Vector4d;
	typedef		Vector4t<int, glm::highp>		Vector4i;
	typedef		Vector4t<uint, glm::highp>		Vector4u;
}