#pragma once

#include <string>
#include <glm/gtc/quaternion.hpp>
#include "./Vector3.h"

namespace Esteem
{
	class Quaternion : public glm::quat
	{
	public:
		Quaternion()
			: glm::quat(glm::identity<glm::quat>())
		{}

		/// \brief conversion constructor
		Quaternion(const glm::quat& q)
			: glm::quat(q)
		{}

		Quaternion(float x, float y, float z, float w)
			: glm::quat(x, y, z, w)
		{}

		/// \brief Create Quaternion facing vector
		Quaternion(const Vector3& angles)
			: glm::quat(static_cast<const glm::vec3&>(angles))
		{}
		
		/// \brief Create Quaternion facing vector
		Quaternion(const Vector3& forward, const Vector3& up)
			: glm::quat(glm::quatLookAt(static_cast<const glm::vec3&>(forward), static_cast<const glm::vec3&>(up)))
		{}

		inline Quaternion LocalRotate(const float angle, const glm::vec3& axis) const { return *this * glm::angleAxis(angle, axis); }
		inline Quaternion WorldRotate(const float angle, const glm::vec3& axis) const { return glm::angleAxis(angle, axis) * *this; }

		//operator glm::quat&() { return *this; }
		operator std::string() const { return "Quaternion(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ", " + std::to_string(w) + ")"; }
	};
}