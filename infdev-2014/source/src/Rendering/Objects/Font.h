#pragma once

#include <unordered_map>
#include <cppu/cgc/ref/unordered_map.h>
#include <string_view>

#include <Rendering/Objects/Texture2D.h>
#include <General/Rect.h>
#include <Utils/File.h>
#include <Utils/Hash.h>

namespace Esteem
{
	namespace details
	{
		struct FontCharacter
		{
			Rect<float> uvData;
			float advance;
			glm::vec2 offset;
			glm::vec2 size;

			FontCharacter(const Rect<float>& uvData, const glm::vec2& size, float advance, const glm::vec2& offset)
				: uvData(uvData)
				, size(size)
				, advance(advance)
				, offset(offset)
			{
			}

			FontCharacter()
				: uvData(0, 0, 0, 0)
				, size(0, 0)
				, advance(0)
				, offset(0)
			{

			}
		};

		struct FontInfo
		{
			float lineHeight;
			float spaceWidth;
			float ascent;
			float descent;
			float lineGap;
			float fontSize;

			FontInfo()
			{ }
		};
	}

	class Font
	{
		friend cgc::constructor;
	private:
		static cgc::ref::unordered_map<hash_t, Font> globalFonts;

		std::string name;
		cgc::strong_ptr<Material> material;
		std::vector<details::FontCharacter> characters;
		details::FontInfo info;

		Font(const std::string& path, const cgc::strong_ptr<Material>& material, std::vector<details::FontCharacter>&& characters, const details::FontInfo& fontInfo);

	public:
		Font();

		static cgc::strong_ptr<Font> LoadFont(std::string_view path);

		bool IsLoaded() { return material; }
		const cgc::strong_ptr<Material>& GetMaterial() { return material; }

		const std::vector<details::FontCharacter>& GetFontCharacterInfo() const { return characters; }
		const details::FontInfo& GetFontInfo() const { return info; }
	};
}