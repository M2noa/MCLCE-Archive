#pragma once

#include "Rendering/rtypes.h"

#include "General/CommonTypes.h"
#include <glm/glm.hpp>
#include <Rendering/Objects/IVBO.h>

namespace Esteem
{
	struct ModelVertexDataA
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec3 tangent;
		glm::vec3 color;
		glm::vec2 uv;
		glm::uvec4 bones;
		glm::vec4 weights;

		ModelVertexDataA(const glm::vec3& position)
			: position(position)
			, bones(glm::uvec4(0, 0, 0, 0))
			, weights(glm::vec4(0, 0, 0, 0))
		{
		}

		ModelVertexDataA()
			: bones(glm::uvec4(0, 0, 0, 0))
			, weights(glm::vec4(0, 0, 0, 0))
		{
		}

		static void SetAttributes(IVBO* vbo)
		{
			vbo->AddVertexAttribute("vertex", 3, TYPE_SINGLE, sizeof(ModelVertexDataA), (void*)offsetof(ModelVertexDataA, position));
			vbo->AddVertexAttribute("normal", 3, TYPE_SINGLE, sizeof(ModelVertexDataA), (void*)offsetof(ModelVertexDataA, normal));
			vbo->AddVertexAttribute("tangent", 3, TYPE_SINGLE, sizeof(ModelVertexDataA), (void*)offsetof(ModelVertexDataA, tangent));
			vbo->AddVertexAttribute("color", 3, TYPE_SINGLE, sizeof(ModelVertexDataA), (void*)offsetof(ModelVertexDataA, color));
			vbo->AddVertexAttribute("uv", 2, TYPE_SINGLE, sizeof(ModelVertexDataA), (void*)offsetof(ModelVertexDataA, uv));
			vbo->AddVertexAttribute("boneIds", 4, TYPE_UINT, sizeof(ModelVertexDataA), (void*)offsetof(ModelVertexDataA, bones), IVBO::Mapping::INTEGER);
			vbo->AddVertexAttribute("weights", 4, TYPE_SINGLE, sizeof(ModelVertexDataA), (void*)offsetof(ModelVertexDataA, weights));
		}
	};
}