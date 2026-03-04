#pragma once

#include "Rendering/rtypes.h"

#include "General/CommonTypes.h"
#include "General/Color.h"
#include <glm/glm.hpp>
#include <Rendering/Objects/IVBO.h>

namespace Esteem
{
	struct OverlayVertexData
	{
		glm::vec4 position;
		Esteem::Color8u color;
		glm::vec3 normal;
		glm::vec2 uv;

		OverlayVertexData(const glm::vec4& position, const Esteem::Color8u& color, const glm::vec3& normal, const glm::vec2& uv)
			: position(position)
			, color(color)
			, normal(normal)
			, uv(uv)
		{
		}

		OverlayVertexData()
		{
		}
		
		static void SetAttributes(IVBO* vbo)
		{
			vbo->AddVertexAttribute("vertex", 4, TYPE_SINGLE, sizeof(OverlayVertexData), (void*)offsetof(OverlayVertexData, position));
			vbo->AddVertexAttribute("color", 4, TYPE_UBYTE, sizeof(OverlayVertexData), (void*)offsetof(OverlayVertexData, color), IVBO::Mapping::FLOAT_NORMALIZED);
			vbo->AddVertexAttribute("normal", 3, TYPE_SINGLE, sizeof(OverlayVertexData), (void*)offsetof(OverlayVertexData, normal));
			vbo->AddVertexAttribute("uv", 2, TYPE_SINGLE, sizeof(OverlayVertexData), (void*)offsetof(OverlayVertexData, uv));
		}
	};
}