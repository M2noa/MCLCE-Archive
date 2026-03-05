#pragma once

#include "stdafx.h"
#include <string>
#include <vector>
#include <glm/vec2.hpp>
#include <cppu/cgc/pointers.h>

#include "AbstractBuffer.h"
#include "General/Color.h"
#include "../RenderingFactory.h"
#include "../rtypes.h"


namespace Esteem
{
	class Image;

	class TextureCube : public AbstractBuffer
	{
	friend class RenderingFactory;
	public:
		struct Settings
		{
			glm::uvec2 size;
			TEXTURE_REPEAT repeat;
			TEXTURE_FILTER textureFilter;
			TEXTURE_FORMAT format;
			std::bitset<8> flags;

			Settings(const ConstructSettings::TextureCube& settings)
				: size(settings.size)
				, repeat(settings.repeat)
				, textureFilter(settings.textureFilter)
				, format(settings.format)
				, flags(settings.mipmapped + (settings.keepImageOnCPU << 1))
			{ }

			bool IsMipMapped() const { return flags[0]; }
			bool KeepImageOnCPU() const { return flags[1]; }
		};
	private:
		std::string name;
		Settings settings;
		std::vector<cgc::strong_ptr<Image>> images;
		glm::uvec2 size;

	public:
		TextureCube(const Settings& settings, uint id, const std::string& name);
		TextureCube(const Settings& settings, uint id, const std::string& name, const std::vector<cgc::strong_ptr<Image>>& images = std::vector<cgc::strong_ptr<Image>>());

		inline uint GetID() const { return id; }
		inline const std::string& GetName() const { return name; }
		inline const std::vector<cgc::strong_ptr<Image>>& GetImages() const { return images; }
		inline const glm::uvec2& GetSize() const { return size; }
	};
}