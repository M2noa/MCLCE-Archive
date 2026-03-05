#pragma once

#include <glm/glm.hpp>
#include <array>

#ifndef M_PI
#define M_PI	3.14159265358979323846264338328
#endif
#ifndef M_HALF_PI
#define M_HALF_PI	1.57079632679489661923132169164
#endif
#ifndef M_TAU
#define M_TAU	6.28318530717958647692528676656
#endif

namespace Math
{
	static constexpr double PI = M_PI;
	static constexpr double TAU = M_TAU;
	static constexpr double TO_RADIANS = PI / 180.0;
	static constexpr double TO_DEGREES = 180.0 / PI;

	static glm::mat4 GetReflectionMatrix(const glm::vec4& plane)
	{
		glm::mat4 mat;
		float a = plane.x;
		float b = plane.y;
		float c = plane.z;
		float d = plane.w;

		mat[0][0] = 1 - 2 * a * a;	mat[0][1] = -2 * a * b;		mat[0][2] = -2 * a * c;		mat[0][3] = -2 * a * d;
		mat[1][0] = -2 * a * b;		mat[1][1] = 1 - 2 * b * b;	mat[1][2] = -2 * b * c;		mat[1][3] = -2 * b * d;
		mat[2][0] = -2 * a * c;		mat[2][1] = -2 * b * c;		mat[2][2] = 1 - 2 * c * c;	mat[2][3] = -2 * c * d;
		mat[3][0] = 0.0f;			mat[3][1] = 0.0f;			mat[3][2] = 0.0f;			mat[3][3] = 1.0f;

		return mat;
	}

	static std::array<glm::vec2, 2> CalculateCircleLineIntersectionPoints(const glm::vec2& circlePosition, const float circleRadius, glm::vec2 linePoint1, glm::vec2 linePoint2)
	{
		float r = circleRadius;
		float r2 = r * r;

		if (true) // TODO: determinant check
		{
			linePoint1 -= circlePosition;
			linePoint2 -= circlePosition;

			float d = linePoint1.x * linePoint2.y - linePoint2.x * linePoint1.y;
			float dx = linePoint2.x - linePoint1.x;
			float dy = linePoint2.y - linePoint1.y;

			float dysign = dy < 0 ? -1.f : 1.f;
			float dr = dx * dx + dy * dy;
			float drinv = 1.f / dr;
			float ds = sqrt(r2 * dr - (d * d));

			float ddy = d * dy;
			float mddx = -d * dx;
			float dzxs = dysign * dx * ds;
			float adyds = abs(dy) * ds;

			std::array<glm::vec2, 2> positions
			{
				circlePosition + glm::vec2((ddy - dzxs) * drinv, (mddx - adyds) * drinv),
				circlePosition + glm::vec2((ddy + dzxs) * drinv, (mddx + adyds) * drinv)
			};

			return positions;
		};

		return {};
	}

	static glm::vec3 GetReflectionVector(const glm::vec3& direction, const glm::vec3& normal)
	{
		return direction - normal * (2.f * glm::dot(direction, normal));
	}

	static glm::vec3 GetParallelVector(const glm::vec3& direction, const glm::vec3& normal)
	{
		return normal * glm::dot(direction, normal);
	}

	static glm::vec3 GetPerpendicularVector(const glm::vec3& direction, const glm::vec3& normal)
	{
		return direction - glm::dot(direction, normal) * normal;
	}

	static glm::vec4 NormalizeFrustumPlane(const glm::vec4& plane)
	{
		// only normalize by the normal part, not the distance
		auto invMagnitude = glm::vec4::value_type(1.0) / glm::sqrt(plane.x * plane.x + plane.y * plane.y + plane.z * plane.z);

		return {
			plane.x * invMagnitude,
			plane.y * invMagnitude,
			plane.z * invMagnitude,
			plane.w * invMagnitude
		};
	}

	static glm::vec3 ToEulerAngles(glm::quat q)
	{
		glm::vec3 angles;
		typedef glm::vec3::value_type Scalar;

		Scalar q2sq = q.z * q.z;
		Scalar t0 = Scalar(-2.0) * (q2sq + q.w * q.w) + Scalar(1.);
		Scalar t1 = Scalar(+2.0) * (q.y * q.z + q.x * q.w);
		Scalar t2 = Scalar(-2.0) * (q.y * q.w - q.x * q.z);
		Scalar t3 = Scalar(+2.0) * (q.z * q.w + q.x * q.y);
		Scalar t4 = Scalar(-2.0) * (q.y * q.y + q2sq) + Scalar(1.);

		t2 = t2 > Scalar(1.) ? Scalar(1.) : t2;
		t2 = t2 < Scalar(-1.) ? Scalar(-1.) : t2;

		angles.x = asin(t2);
		angles.y = atan2(t1, t0);
		angles.z = atan2(t3, t4);

		return angles;
	}

	/// \brief convert degrees to radians
	template <typename T>
	inline constexpr static T ToRadians(const T& degrees)
	{
		return T(degrees * TO_RADIANS);
	}

	/// \brief convert radians to degrees
	template <typename T>
	static constexpr float ToDegrees(const T& radians)
	{
		return T(radians * TO_DEGREES);
	}

	inline constexpr static int Square(int a)
	{
		return a * a;
	}

	inline constexpr static int Cubic(int a)
	{
		return a * a * a;
	}

	inline constexpr static float Lerp(float from, float to, float delta)
	{
		return from + (to - from) * delta;
	}
}