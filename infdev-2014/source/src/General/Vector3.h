#pragma once

#include "stdafx.h"
#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

namespace Esteem
{
	template<typename T, glm::precision P>
	struct Vector3t : public glm::vec<3, T, P>
	{
	public:
		static const Vector3t<T, P> zero;
		static const Vector3t<T, P> one;
		static const Vector3t<T, P> forward;
		static const Vector3t<T, P> up;
		static const Vector3t<T, P> right;
		
		Vector3t();

		/// \brief conversion constructor
		template <typename T2, typename = typename std::enable_if_t<std::is_assignable_v<T&, T2>>>
		Vector3t(const glm::vec<3, T2, P>& v);

		Vector3t(T xyz);
		Vector3t(T x, T y, T z);

		// converting constructor
		template <typename T2, glm::precision Q>
		explicit Vector3t(const Vector3t<T2, Q>& v)
			: glm::vec<3, T, P>(v.x, v.y, v.z) {}

		void Normalize();
		Vector3t<T, P> Normalized() const;

		float Dot(const Vector3t<T, P>& b) const;
		Vector3t<T, P> Cross(const Vector3t<T, P>& b) const;

		static float Dot(const Vector3t<T, P>& a, const Vector3t<T, P>& b);
		static Vector3t<T, P> Cross(const Vector3t<T, P>& a, const Vector3t<T, P>& b);

		static float Distance(const Vector3t<T, P>& a, const Vector3t<T, P>& b);
		static float Distance2(const Vector3t<T, P>& a, const Vector3t<T, P>& b);
		static Vector3t<T, P> Lerp(const Vector3t<T, P>& from, const Vector3t<T, P>& to, float t);
		static Vector3t<T, P> Abs(const Vector3t<T, P>& v);

		float Length() const;
		float Length2() const;

		operator glm::vec<3, T, P>&() { return *this; }
		operator const glm::vec<3, T, P>&() const { return *this; }

		operator glm::vec<2, T, P>& () { return *this; }
		operator const glm::vec<2, T, P>& () const { return *this; }

		void operator*=(T value) { this->x *= value; this->y *= value; this->z *= value; }
		void operator/=(T value) { this->x /= value; this->y /= value; this->z /= value; }

		void operator+=(glm::vec<3, T, P> b) { this->x += b.x; this->y += b.y; this->z += b.z; }
		void operator-=(glm::vec<3, T, P> b) { this->x -= b.x; this->y -= b.y; this->z -= b.z; }

		Vector3t<T, P> operator*(T value) { return Vector3t<T, P>(this->x * value, this->y * value, this->z * value); }
		Vector3t<T, P> operator/(T value) { return Vector3t<T, P>(this->x / value, this->y / value, this->z / value); }

		Vector3t<T, P> operator*(const glm::vec<3, T, P>& b) { return Vector3t<T, P>(this->x * b.x, this->y * b.y, this->z * b.z); }
		Vector3t<T, P> operator/(const glm::vec<3, T, P>& b) { return Vector3t<T, P>(this->x / b.x, this->y / b.y, this->z / b.z); }

		Vector3t<T, P> operator+(const glm::vec<3, T, P>& b) { return Vector3t<T, P>(this->x + b.x, this->y + b.y, this->z + b.z); }
		Vector3t<T, P> operator-(const glm::vec<3, T, P>& b) { return Vector3t<T, P>(this->x - b.x, this->y - b.y, this->z - b.z); }

		inline explicit operator std::string() const { return "Vector3(" + std::to_string(this->x) + ", " + std::to_string(this->y) + ", " + std::to_string(this->z) + ")"; }
	};
}

#include "./Vector3.inl"