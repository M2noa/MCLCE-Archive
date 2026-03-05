#pragma once

#include "stdafx.h"
#include <string>
#include <glm/vec2.hpp>

#include "AbstractBuffer.h"

namespace Esteem
{
	class Texture3D : public AbstractBuffer
	{
	private:
		std::string path;
		glm::uvec2 size;

		Texture3D();

	public:

		inline uint GetID() const { return id; }
		inline const std::string& GetPath() const { return path; }
		inline const glm::uvec2& GetSize() const { return size; }
	};
}