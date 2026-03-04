#pragma once

#include "stdafx.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "General/CommonTypes.h"

namespace Esteem
{
	// align this struct by every 16 bytes (ie: 4 times int/float etc)
	struct alignas(16) LightData // 16 units
	{
		enum LightType : int32
		{
			DIRECTIONAL = 0,
			POINT = 1,
			SPOT = 2,

			NONE_REUSE = std::numeric_limits<int32>::max()
		};

		glm::vec3 position;
		int32 type;
		Color color; // we only store r, g and b as color and the a value as brightness
		// float brightness; we will use color.a to store this value
		glm::vec3 forward;
		float distance;
		int32 shadowMapIndex;
		int32 shadowInfoIndex;

		LightData()
			: type(LightType::POINT)
			, color(1.f, 1.f, 1.f, 1.f)
			, distance(1.f)
			, shadowMapIndex(-1)
			, shadowInfoIndex(0)
		{}

		inline void SetShadowMapIndex(int32 shadowMapIndex) { this->shadowMapIndex = shadowMapIndex; }
	};

	struct LightRenderData
	{
		LightData* data;
		glm::mat4* matrix;
		uint32_t index;
	};
}