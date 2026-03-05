#pragma once

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Utils/Debug.h"
#include "General/Color.h"

namespace Esteem
{
	struct DevRenderObject
	{
	public:
		enum GeometryType
		{
			BOX = 0x1,
			CYLINDER = 0x2,
			SPHERE = 0x3,
			CAPSULE = 0x4,
			CONE = 0x5,
			PYRAMIDE = 0x6,
			QUAD = 0x7,

			TRIANGLES = 0x255
		};

		enum RenderType
		{
			LINES,
			FILLED,
			FILLED_AND_LINES,
			TEXTURED
		};

		glm::vec3 position;
		glm::quat rotation;
		glm::vec3 scale;
		Color8u color;

		GeometryType geometryType;
		RenderType renderType;

		DevRenderObject(GeometryType object, RenderType renderType, glm::vec3 position, glm::quat rotation, glm::vec3 scale, Color8u color)
			: geometryType(GeometryType::BOX)
			, renderType(renderType)
			, position(position)
			, rotation(rotation)
			, scale(scale)
			, color(color)
		{

		}
	};
}