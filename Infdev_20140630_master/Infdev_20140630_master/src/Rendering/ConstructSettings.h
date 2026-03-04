#pragma once

#include <glm/vec2.hpp>
#include "General/Color.h"
#include "rtypes.h"

namespace Esteem
{
	namespace ConstructSettings
	{
		struct Buffer
		{
			std::size_t size;
			TEXTURE_FORMAT format;
			bool keepImageOnCPU;

			Buffer()
				: size(0)
				, format(TEXTURE_FORMAT::RGBA)
				, keepImageOnCPU(false)
			{}
		};

		struct Texture1D
		{
			uint size;
			TEXTURE_REPEAT repeat;
			TEXTURE_FILTER textureFilter;
			TEXTURE_FILTER mipMapFilter;
			TEXTURE_FORMAT format;
			Color borderColor;
			bool mipmapped;
			bool keepImageOnCPU;
			bool custom;

			Texture1D()
				: size(0)
				, repeat(TEXTURE_REPEAT::CLAMP_TO_EDGE)
				, textureFilter(TEXTURE_FILTER::LINEAR)
				, mipMapFilter(TEXTURE_FILTER::LINEAR_MIPMAP_LINEAR)
				, format(TEXTURE_FORMAT::RGBA)
				, borderColor(0, 0, 0)
				, mipmapped(true)
				, keepImageOnCPU(false)
				, custom(false)
			{}
		};

		struct Texture2D
		{
			glm::uvec2 size;
			TEXTURE_REPEAT repeat;
			TEXTURE_FILTER textureFilter;
			TEXTURE_FILTER mipMapFilter;
			TEXTURE_FORMAT format;
			Color borderColor;
			bool mipmapped;
			bool keepImageOnCPU;
			bool custom;

			Texture2D()
				: size(0,0)
				, repeat(TEXTURE_REPEAT::CLAMP_TO_EDGE)
				, textureFilter(TEXTURE_FILTER::LINEAR)
				, mipMapFilter(TEXTURE_FILTER::LINEAR_MIPMAP_LINEAR)
				, format(TEXTURE_FORMAT::RGBA)
				, borderColor(0, 0, 0)
				, mipmapped(true)
				, keepImageOnCPU(false)
				, custom(false)
			{}
		};

		typedef Texture2D Texture3D;
		typedef Texture2D TextureCube;
	}
}