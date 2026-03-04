#pragma once

#include "stdafx.h"
#include <string>
#include <bitset>
#include <glm/vec2.hpp>
#include <cppu/cgc/pointers.h>

#include "AbstractBuffer.h"
#include "General/Color.h"
#include "../RenderingFactory.h"
#include "../rtypes.h"

namespace Esteem
{
	class RenderingFactory;
	class Image;

	class Texture2D : public AbstractBuffer
	{
		friend class RenderingFactory;
		friend class cgc::constructor;
	public:
		struct Settings
		{
			glm::uvec2 size;
			TEXTURE_REPEAT repeat;
			TEXTURE_FILTER textureFilter;
			TEXTURE_FORMAT format;
			std::bitset<8> flags;

			Settings(const ConstructSettings::Texture2D& settings)
				: size(settings.size)
				, repeat(settings.repeat)
				, textureFilter(settings.textureFilter)
				, format(settings.format)
				, flags(uint8(settings.mipmapped) | uint8(settings.keepImageOnCPU << 1))
			{ }

			bool IsMipMapped() const { return flags[0]; }
			bool KeepImageOnCPU() const { return flags[1]; }
		};
		
	private:
		std::string path;
		cgc::strong_ptr<Image> image;
		Settings settings;

		Texture2D(const Settings& settings);
		Texture2D(const Settings& settings, uint id, const std::string& path, const cgc::strong_ptr<Image>& image = cgc::strong_ptr<Image>());

	public:
		~Texture2D();

		static cgc::strong_ptr<Texture2D> Construct(const ConstructSettings::Texture2D& settings, const cgc::strong_ptr<Image>& image = cgc::strong_ptr<Image>());
		static cgc::strong_ptr<Texture2D> Construct(const ConstructSettings::Texture2D& settings, const std::string& name);
		static cgc::strong_ptr<Texture2D> GetTexture(const std::string& name);

		inline uint GetID() const { return id; }
		inline const cgc::strong_ptr<Image>& GetImage() const { return image; }
		inline const std::string& GetPath() const { return path; }
		inline const Settings& GetSettings() const { return settings; }

		static void UpdateImage(const cgc::strong_ptr<Texture2D>& texture, const cgc::strong_ptr<Image>& image, const glm::uvec2& position = glm::uvec2(0, 0));

	};
}