#pragma once

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Model/ModelFactory.h"

namespace Esteem
{
	namespace Animation
	{
		struct MixedFrame
		{
			glm::quat r;
			glm::vec3 s, t;

			MixedFrame()
				: r(), s(), t()
			{ }

			MixedFrame(const glm::quat& rotation, const glm::vec3& scale, const glm::vec3& translation)
				: r(rotation)
				, s(scale)
				, t(translation)
			{ }

			MixedFrame(const Model::BoneData& boneData)
				: MixedFrame(boneData.defaults.rotation, boneData.defaults.scale, boneData.defaults.translation)
			{ }
		};
	}
}