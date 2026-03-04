/*
* Conversion codes are from the GLM math library (https://github.com/g-truc/glm, MIT licensed)
* combined here to lower overhead by combining the operations
*/

#pragma once

#include <glm/mat4x4.hpp>

#include "Quaternion.h"
#include "Vector3.h"

namespace Esteem
{
	template<int C, int R, typename T = float>
	class Matrix : public glm::mat<C, R, T>
	{
	protected:
		Matrix()
		{
			memset(&this[0][0], 0, sizeof(*this));
		}

		// clean no initialization
		Matrix(std::nullptr_t)
		{}

		Matrix(T def)
			: glm::mat<C, R, T>(def)
		{ }

		template <int C2, int R2, typename = typename std::enable_if_t<(C2 <= C && R2 <= R)>>
		Matrix(const glm::mat<C2, R2, T>& copy)
		{
			for (size_t c = 0; c < C2; ++c)
				reinterpret_cast<glm::vec<R2, T>&>((*this)[c]) = copy[c];
		}

		template <typename = typename std::enable_if_t<(R > 3 && C > 2)>>
		Matrix(const Quaternion& r, const Vector3& s, const Vector3& t)
			: Matrix(r, s)
		{
			if constexpr(C > 3)
				(*this)[0][3] = (*this)[1][3] = (*this)[2][3] = T(0);

			(*this)[3][0] = t[0];
			(*this)[3][1] = t[1];
			(*this)[3][2] = t[2];
			(*this)[3][3] = T(1);
		}

		template <typename = typename std::enable_if_t<(R > 2 && C > 2)>>
		Matrix(const Quaternion& r, const Vector3& s)
		{
			T rxx(r.x * r.x);
			T ryy(r.y * r.y);
			T rzz(r.z * r.z);
			T rxz(r.x * r.z);
			T rxy(r.x * r.y);
			T ryz(r.y * r.z);
			T rwx(r.w * r.x);
			T rwy(r.w * r.y);
			T rwz(r.w * r.z);

			(*this)[0][0] = s[0] * (T(1) - T(2) * (ryy + rzz));
			(*this)[0][1] = s[0] * (T(2) * (rxy + rwz));
			(*this)[0][2] = s[0] * (T(2) * (rxz - rwy));

			(*this)[1][0] = s[1] * (T(2) * (rxy - rwz));
			(*this)[1][1] = s[1] * (T(1) - T(2) * (rxx + rzz));
			(*this)[1][2] = s[1] * (T(2) * (ryz + rwx));

			(*this)[2][0] = s[2] * (T(2) * (rxz + rwy));
			(*this)[2][1] = s[2] * (T(2) * (ryz - rwx));
			(*this)[2][2] = s[2] * (T(1) - T(2) * (rxx + ryy));
		}

		template <typename = typename std::enable_if_t<(R > 2 && C > 2)>>
		Matrix(const Quaternion& r)
		{
			T rxx(r.x * r.x);
			T ryy(r.y * r.y);
			T rzz(r.z * r.z);
			T rxz(r.x * r.z);
			T rxy(r.x * r.y);
			T ryz(r.y * r.z);
			T rwx(r.w * r.x);
			T rwy(r.w * r.y);
			T rwz(r.w * r.z);

			(*this)[0][0] = T(1) - T(2) * (ryy + rzz);
			(*this)[0][1] = T(2) * (rxy + rwz);
			(*this)[0][2] = T(2) * (rxz - rwy);

			(*this)[1][0] = T(2) * (rxy - rwz);
			(*this)[1][1] = T(1) - T(2) * (rxx + rzz);
			(*this)[1][2] = T(2) * (ryz + rwx);

			(*this)[2][0] = T(2) * (rxz + rwy);
			(*this)[2][1] = T(2) * (ryz - rwx);
			(*this)[2][2] = T(1) - T(2) * (rxx + ryy);
		}

	public:
		static Matrix<C, R, T> Identity()
		{
			return Matrix<C, R, T>(T(1));
		}
	};

	class Matrix4x4 : public Matrix<4, 4>
	{
	public:
		Matrix4x4(const Quaternion& r, const Vector3& s, const Vector3& t)
			: Matrix<4, 4>(r, s, t)
		{ }

		Matrix4x4(const glm::mat3& r, const Vector3& s, const Vector3& t)
			: Matrix<4, 4, float>(nullptr)
		{
			reinterpret_cast<glm::vec3&>((*this)[0]) = s[0] * r[0]; (*this)[0][3] = 0.f;
			reinterpret_cast<glm::vec3&>((*this)[1]) = s[1] * r[1]; (*this)[1][3] = 0.f;
			reinterpret_cast<glm::vec3&>((*this)[2]) = s[2] * r[2]; (*this)[2][3] = 0.f;
			reinterpret_cast<glm::vec3&>((*this)[3]) = t; (*this)[3][3] = 1.f;
		}
	};
	
	class Matrix3x3 : public Matrix<3, 3>
	{
	public:
		Matrix3x3(const Quaternion& r)
			: Matrix<3, 3>(r)
		{ }
		
		Matrix3x3(const Quaternion& r, const Vector3& s)
			: Matrix<3, 3>(r, s)
		{ }
	};

	struct alignas(sizeof(__m128)) float4
	{
		union
		{
			float x, y, z, w;
			__m128 xmm;
		};

		float& operator[] (size_t i)
		{
			return reinterpret_cast<float*>(&x)[i];
		}
	};

	struct float4x3
	{
		typedef float T;

		float4 values[4];		

		float4x3(const Quaternion& r, const Vector3& s, const Vector3& t)
		{
			T rxx(r.x * r.x);
			T ryy(r.y * r.y);
			T rzz(r.z * r.z);
			T rxz(r.x * r.z);
			T rxy(r.x * r.y);
			T ryz(r.y * r.z);
			T rwx(r.w * r.x);
			T rwy(r.w * r.y);
			T rwz(r.w * r.z);

			values[0][0] = s[0] * (T(1) - T(2) * (ryy + rzz));
			values[0][1] = s[0] * (T(2) * (rxy - rwz));
			values[0][2] = s[0] * (T(2) * (rxz + rwy));
			values[0][3] = t[0];

			values[1][0] = s[0] * (T(2) * (rxy + rwz));
			values[1][1] = s[0] * (T(1) - T(2) * (rxx + rzz));
			values[1][2] = s[0] * (T(2) * (ryz - rwx));
			values[1][3] = t[1];

			values[2][0] = s[0] * (T(2) * (rxz - rwy));
			values[2][1] = s[0] * (T(2) * (ryz + rwx));
			values[2][2] = s[0] * (T(1) - T(2) * (rxx + ryy));
			values[2][3] = t[2];
		}
	};
}

namespace glm
{
	namespace gtx
	{
		template<int C, typename T, typename = typename std::enable_if_t<(C == 3 || C == 4)>>
		glm::mat<3, 4, T> xmul(const glm::mat<3, 4, T>& m1, const glm::mat<C, 4, T>& m2)
		{
			glm::mat<3, 4, T> m;

			__m128 xmm0, xmm1;
			constexpr union { int ints[4]; __m128 xmm; } pos_mask = { 0, 0, 0, ~0 };

			__m128 xmm_m1[3]
			{
				_mm_loadu_ps(&m1[0][0]),
				_mm_loadu_ps(&m1[1][0]),
				_mm_loadu_ps(&m1[2][0])
			};

			__m128 xmm_m2[3]
			{
				_mm_loadu_ps(&m2[0][0]),
				_mm_loadu_ps(&m2[1][0]),
				_mm_loadu_ps(&m2[2][0])
			};

			for (size_t i = 0; i < 3; ++i)
			{
				__m128 xmm_m2_ps1[3]
				{
					_mm_shuffle_ps(xmm_m2[i], xmm_m2[i], _MM_SHUFFLE(0, 0, 0, 0)),
					_mm_shuffle_ps(xmm_m2[i], xmm_m2[i], _MM_SHUFFLE(1, 1, 1, 1)),
					_mm_shuffle_ps(xmm_m2[i], xmm_m2[i], _MM_SHUFFLE(2, 2, 2, 2))
				};
				
				xmm0 = _mm_mul_ps(xmm_m1[0], xmm_m2_ps1[0]);
				xmm1 = _mm_mul_ps(xmm_m1[1], xmm_m2_ps1[1]);
				xmm0 = _mm_add_ps(xmm0, xmm1);

				xmm1 = _mm_mul_ps(xmm_m1[2], xmm_m2_ps1[2]);
				xmm0 = _mm_add_ps(xmm0, xmm1);
				
				xmm1 = _mm_and_ps(xmm_m2[i], pos_mask.xmm);
				xmm0 = _mm_add_ps(xmm0, xmm1);

				_mm_storeu_ps(&m[i][0], xmm0);
			}
			return m;
		}

		template<int C, typename T, typename = typename std::enable_if_t<(C == 3 || C == 4)>>
		glm::mat<C, 4, T> to_row_major_mat(const glm::qua<T>& r, const glm::vec<3, T>& s, const glm::vec<3, T>& t)
		{
			glm::mat<C, 4, T> m;

			T rxx(r.x * r.x);
			T ryy(r.y * r.y);
			T rzz(r.z * r.z);
			T rxz(r.x * r.z);
			T rxy(r.x * r.y);
			T ryz(r.y * r.z);
			T rwx(r.w * r.x);
			T rwy(r.w * r.y);
			T rwz(r.w * r.z);

			m[0][0] = s[0] * (T(1) - T(2) * (ryy + rzz));
			m[0][1] = s[1] * (T(2) * (rxy - rwz));
			m[0][2] = s[2] * (T(2) * (rxz + rwy));
			m[0][3] = t[0];

			m[1][0] = s[0] * (T(2) * (rxy + rwz));
			m[1][1] = s[1] * (T(1) - T(2) * (rxx + rzz));
			m[1][2] = s[2] * (T(2) * (ryz - rwx));
			m[1][3] = t[1];

			m[2][0] = s[0] * (T(2) * (rxz - rwy));
			m[2][1] = s[1] * (T(2) * (ryz + rwx));
			m[2][2] = s[2] * (T(1) - T(2) * (rxx + ryy));
			m[2][3] = t[2];

			if constexpr (C > 3)
			{
				m[3][0] = m[3][1] = m[3][2] = T(0);
				m[3][3] = T(1);
			}

			return m;
		}

		template<typename T, qualifier Q, int C = 3, int R = 3, typename = typename std::enable_if_t<(C > 2 && R > 2)>>
		GLM_FUNC_QUALIFIER qua<T, Q> quat_cast_row_major_rsqrt(const mat<C, R, T, Q>& m)
		{
			__m128 q = _mm_set_ps(
				T(1) - m[0][0] - m[1][1] + m[2][2],
				T(1) - m[0][0] + m[1][1] - m[2][2],
				T(1) + m[0][0] - m[1][1] - m[2][2],
				T(1) + m[0][0] + m[1][1] + m[2][2]);

			const __m128 s = _mm_set_ps(
				m[1][0] - m[0][1],
				m[0][2] - m[2][0],
				m[2][1] - m[1][2],
				reinterpret_cast<float&>(q));

			const __m128 signbit = _mm_set_ps1(-0.f);
			const __m128 quarter = _mm_set_ps1(0.25f);
			const __m128 three = _mm_set_ps1(3.0f);
			const __m128 inf = _mm_set_ps1(std::numeric_limits<float>::infinity());

			__m128 r = _mm_rsqrt_ps(q);
			r = _mm_and_ps(r, _mm_cmpneq_ps(r, inf));

			// Newton Raphson Iteration
			__m128 u = _mm_mul_ps(_mm_mul_ps(q, r), r);
			u = _mm_mul_ps(r, _mm_sub_ps(three, u));

			q = _mm_mul_ps(_mm_mul_ps(u, q), quarter);
			q = _mm_or_ps(_mm_and_ps(signbit, s), _mm_andnot_ps(signbit, q)); // (signbit & from) | (~signbit & to)

			return reinterpret_cast<qua<T, Q>&>(q);
		}

		template<typename T, qualifier Q, int C = 3, int R = 3, typename = typename std::enable_if_t<(C > 2 && R > 2)>>
		GLM_FUNC_QUALIFIER qua<T, Q> quat_cast_col_major_rsqrt(const mat<C, R, T, Q>& m)
		{
			__m128 q = _mm_set_ps(
				T(1) - m[0][0] - m[1][1] + m[2][2],
				T(1) - m[0][0] + m[1][1] - m[2][2],
				T(1) + m[0][0] - m[1][1] - m[2][2],
				T(1) + m[0][0] + m[1][1] + m[2][2]);

			const __m128 s = _mm_set_ps(
				m[0][1] - m[1][0],
				m[2][0] - m[0][2],
				m[1][2] - m[2][1],
				reinterpret_cast<float&>(q));

			const __m128 signbit = _mm_set_ps1(-0.f);
			const __m128 quarter = _mm_set_ps1(0.25f);
			const __m128 three = _mm_set_ps1(3.0f);
			const __m128 inf = _mm_set_ps1(std::numeric_limits<float>::infinity());

			__m128 r = _mm_rsqrt_ps(q);
			r = _mm_and_ps(r, _mm_cmpneq_ps(r, inf));

			// Newton Raphson Iteration
			__m128 u = _mm_mul_ps(_mm_mul_ps(q, r), r);
			u = _mm_mul_ps(r, _mm_sub_ps(three, u));

			q = _mm_mul_ps(_mm_mul_ps(u, q), quarter);
			q = _mm_or_ps(_mm_and_ps(signbit, s), _mm_andnot_ps(signbit, q)); // copysign

			return reinterpret_cast<qua<T, Q>&>(q);
		}

		template<typename T, qualifier Q, int C = 3, int R = 3, typename = typename std::enable_if_t<(C > 2 && R > 2)>>
		GLM_FUNC_QUALIFIER qua<T, Q> quat_cast_row_major(const mat<C, R, T, Q>& m)
		{
			__m128 q = _mm_set_ps(
				T(1) - m[0][0] - m[1][1] + m[2][2],
				T(1) - m[0][0] + m[1][1] - m[2][2],
				T(1) + m[0][0] - m[1][1] - m[2][2],
				T(1) + m[0][0] + m[1][1] + m[2][2]);

			const __m128 s = _mm_set_ps(
				m[1][0] - m[0][1],
				m[0][2] - m[2][0],
				m[2][1] - m[1][2],
				reinterpret_cast<float&>(q));

			const __m128 signbit = _mm_set_ps1(-0.f);

			q = _mm_mul_ps(_mm_sqrt_ps(q), _mm_set_ps1(0.5f));
			q = _mm_or_ps(_mm_and_ps(signbit, s), _mm_andnot_ps(signbit, q)); // (signbit & from) | (~signbit & to)

			return reinterpret_cast<qua<T, Q>&>(q);
		}

		template<typename T, qualifier Q, int C = 3, int R = 3, typename = typename std::enable_if_t<(C > 2 && R > 2)>>
		GLM_FUNC_QUALIFIER qua<T, Q> quat_cast_col_major(const mat<C, R, T, Q>& m)
		{
			__m128 q = _mm_set_ps(
				T(1) - m[0][0] - m[1][1] + m[2][2],
				T(1) - m[0][0] + m[1][1] - m[2][2],
				T(1) + m[0][0] - m[1][1] - m[2][2],
				T(1) + m[0][0] + m[1][1] + m[2][2]);

			const __m128 s = _mm_set_ps(
				m[0][1] - m[1][0],
				m[2][0] - m[0][2],
				m[1][2] - m[2][1],
				reinterpret_cast<float&>(q));

			const __m128 signbit = _mm_set_ps1(-0.f);

			q = _mm_mul_ps(_mm_sqrt_ps(q), _mm_set_ps1(0.5f));
			q = _mm_or_ps(_mm_and_ps(signbit, s), _mm_andnot_ps(signbit, q)); // (signbit & from) | (~signbit & to)

			return reinterpret_cast<qua<T, Q>&>(q);
		}
	}
}