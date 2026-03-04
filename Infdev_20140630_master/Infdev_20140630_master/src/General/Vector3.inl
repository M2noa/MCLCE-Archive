#include "./Vector3.h"

#include <iostream>

namespace Esteem
{
	template<typename T, glm::qualifier P>
	Vector3t<T, P>::Vector3t()
		: glm::vec<3, T, P>(T(0), T(0), T(0)) {}

	/// \brief conversion constructor
	template<typename T, glm::qualifier P>
	template <typename T2, typename>
	Vector3t<T, P>::Vector3t(const glm::vec<3, T2, P>& v)
		: glm::vec<3, T, P>(v) {}

	template<typename T, glm::qualifier P>
	Vector3t<T, P>::Vector3t(T x, T y, T z)
		: glm::vec<3, T, P>(x, y, z) {}
	
	template<typename T, glm::qualifier P>
	Vector3t<T, P>::Vector3t(T xyz)
		: glm::vec<3, T, P>(xyz, xyz, xyz) {}

	template <typename T, glm::qualifier P>
	inline void Vector3t<T, P>::Normalize()
	{
		*this = glm::normalize(static_cast<const glm::vec<3, T, P>&>(*this));
	}

	template <typename T, glm::qualifier P>
	inline Vector3t<T, P> Vector3t<T, P>::Normalized() const
	{
		return glm::normalize(static_cast<const glm::vec<3, T, P>&>(*this));
	}

	template<typename T, glm::qualifier P>
	inline float Vector3t<T, P>::Dot(const Vector3t<T, P>& b) const
	{
		return this->x * b.x + this->y * b.y + this->z * b.z;
	}

	template<typename T, glm::qualifier P>
	inline Vector3t<T, P> Vector3t<T, P>::Cross(const Vector3t<T, P>& b) const
	{
		return Vector3t<T, P>(
			this->y * b.z - b.y * this->z,
			this->z * b.x - b.z * this->x,
			this->x * b.y - b.x * this->y);
	}

	template<typename T, glm::qualifier P>
	inline float Vector3t<T, P>::Dot(const Vector3t<T, P>& a, const Vector3t<T, P>& b)
	{
		return a.Dot(b);
	}
	
	template<typename T, glm::qualifier P>
	inline Vector3t<T, P> Vector3t<T, P>::Cross(const Vector3t<T, P>& a, const Vector3t<T, P>& b)
	{
		return a.Cross(b);
	}

	template<typename T, glm::qualifier P>
	inline float Vector3t<T, P>::Distance(const Vector3t<T, P>& a, const Vector3t<T, P>& b)
	{
		return (a - b).length();
	}

	template<typename T, glm::qualifier P>
	inline float Vector3t<T, P>::Distance2(const Vector3t<T, P>& a, const Vector3t<T, P>& b)
	{
		return (a - b).length2();
	}

	template<typename T, glm::qualifier P>
	inline Vector3t<T, P> Vector3t<T, P>::Lerp(const Vector3t<T, P>& from, const Vector3t<T, P>& to, float t)
	{
		return from + (to - from) * t;
	}

	template<typename T, glm::qualifier P>
	inline float Vector3t<T, P>::Length() const
	{
		return glm::sqrt(this->Length2());
	}

	template<typename T, glm::qualifier P>
	inline float Vector3t<T, P>::Length2() const
	{
		return this->x * this->x + this->y * this->y + this->z * this->z;
	}

	template<typename T, glm::qualifier P>
	inline Vector3t<T, P> Vector3t<T, P>::Abs(const Vector3t<T, P>& v)
	{
		return glm::abs(static_cast<const glm::vec3&>(v));
	}

	template<typename T, glm::qualifier P>
	std::ostream& operator<<(std::ostream& os, const Vector3t<T, P>& v)
	{
		os << "Vector3(" << v.x << ", " << v.y << ", " << v.z << ")";
		return os;
	}

	template <typename T, glm::qualifier P>
	const Vector3t<T, P> Vector3t<T, P>::zero = Vector3t<T, P>(0, 0, 0);
	template <typename T, glm::qualifier P>
	const Vector3t<T, P> Vector3t<T, P>::one = Vector3t<T, P>(1, 1, 1);
	template <typename T, glm::qualifier P>
	const Vector3t<T, P> Vector3t<T, P>::forward = Vector3t<T, P>(0, 0, 1);
	template <typename T, glm::qualifier P>
	const Vector3t<T, P> Vector3t<T, P>::up = Vector3t<T, P>(0, 1, 0);
	template <typename T, glm::qualifier P>
	const Vector3t<T, P> Vector3t<T, P>::right = Vector3t<T, P>(1, 0, 0);

	typedef		Vector3t<float, glm::highp>		Vector3;
	typedef		Vector3							Vector3f;
	typedef		Vector3t<double, glm::highp>	Vector3d;
	typedef		Vector3t<int, glm::highp>		Vector3i;
	typedef		Vector3t<uint, glm::highp>		Vector3u;
}