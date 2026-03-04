#include "Font.h"

#include <Utils/Data.h>
#include <Utils/Debug.h>
#include <Rendering/Objects/Image.h>
#include <Rendering/Objects/Texture2D.h>

#include <stb/stb_truetype.h>

namespace Esteem
{
	cgc::ref::unordered_map<hash_t, Font> Font::globalFonts;
	
	Font::Font()
		: info()
	{

	}
	
	Font::Font(const std::string& path, const cgc::strong_ptr<Material>& material, std::vector<details::FontCharacter>&& characters, const details::FontInfo& fontInfo)
		: name(name)
		, material(material)
		, characters(std::move(characters))
		, info(fontInfo)
	{

	}

	cgc::strong_ptr<Font> Font::LoadFont(std::string_view path)
	{
		std::size_t pos = path.rfind('/');
		std::string_view p = (pos == std::string::npos) ? path : path.substr(pos + 1);
		hash_t pathHash = RT_HASH(p);

		auto it = globalFonts.find(pathHash);
		if (it != globalFonts.end())
			return it->second;

		// load in new font
		if (Data::AssetExists(path))
		{
			std::string fontData = Data::ReadAsset(path);
			if (!fontData.empty())
			{
				// loading font data
				const unsigned char* data = reinterpret_cast<const unsigned char*>(fontData.data());
				std::vector<details::FontCharacter> characters;
				characters.resize(256 - 32); // remove first non-visual ascii characters

				// font processing for usage
				stbtt_fontinfo font;
				font.userdata = nullptr;
				if (stbtt_InitFont(&font, data, stbtt_GetFontOffsetForIndex(data, 0)) != 0)
				{
					details::FontInfo fontInfo;

					// settings for SDF
					int fontHeight = 32, padding = 5, textureWidth = 512, textureHeight = 512;
					uint8 edgeValue = 180;
					float pixelDistanceDecrease = float(edgeValue) / float(padding);

					unsigned char* pixelBuffer = new unsigned char[textureWidth * textureHeight];
					memset(pixelBuffer, 0, textureWidth * textureHeight);
					unsigned char* bitmap;

					float scale = stbtt_ScaleForPixelHeight(&font, float(fontHeight));
					int ascent, descent, lineGap;
					stbtt_GetFontVMetrics(&font, &ascent, &descent, &lineGap);

					fontInfo.fontSize = float(fontHeight);
					fontInfo.ascent = float(ascent) * scale;
					fontInfo.descent = float(-descent) * scale;
					fontInfo.lineGap = float(lineGap) * scale;
					fontInfo.lineHeight = fontInfo.ascent + fontInfo.descent + fontInfo.lineGap;
					fontInfo.spaceWidth = 32.f * scale / 4.f; // default, should be overwritten

					int advance, glyphOffsetX, glyphOffsetY, glyphHeight, glyphWidth;
					int xpos = 0, ypos = 0, maxCharHeight = 0;
					
					int character = 32;
					int glyphIndex = stbtt_FindGlyphIndex(&font, character);
					if (glyphIndex)
					{
						stbtt_GetCodepointHMetrics(&font, character, &advance, 0);
						fontInfo.spaceWidth = float(advance) * scale;
					}

					for (++character; character < 255; ++character)
					{
						glyphIndex = stbtt_FindGlyphIndex(&font, character);
						if (glyphIndex)
						{
							bitmap = stbtt_GetGlyphSDF(&font, scale, glyphIndex, padding, edgeValue, pixelDistanceDecrease, &glyphWidth, &glyphHeight, &glyphOffsetX, &glyphOffsetY);
							if (bitmap)
							{
								stbtt_GetCodepointHMetrics(&font, character, &advance, 0);

								if ((xpos + glyphWidth) > textureWidth)
								{
									ypos += maxCharHeight;
									xpos = maxCharHeight = 0;
								}

								glm::vec2 characterSize(glyphWidth, glyphHeight);
								characters.emplace(characters.begin() + (character - 33), Rect<float>(float(xpos) / float(textureWidth), float(ypos) / float(textureHeight),
									characterSize.x / float(textureWidth), characterSize.y / float(textureHeight)), characterSize, advance * scale, glm::vec2(glyphOffsetX, glyphOffsetY));

								int srcX, srcY, destX = xpos, destY = ypos;
								for (srcY = 0; srcY < glyphHeight; ++srcY, ++destY)
								{
									for (srcX = 0; srcX < glyphWidth; ++srcX, ++destX)
										pixelBuffer[destY * textureWidth + destX] = bitmap[srcY * glyphWidth + srcX];

									destX = xpos;
								}

								stbtt_FreeSDF(bitmap, 0);
								xpos += glyphWidth;
								maxCharHeight = std::max(maxCharHeight, glyphHeight);
							}
						}
					}

					cgc::strong_ptr<Image> image = cgc::construct_new<Image>(IMAGE_FORMAT::R);
					image->Create(textureWidth, textureHeight, pixelBuffer, 1, true);

					stbtt_FreeSDF(0, font.userdata);
					
					cgc::strong_ptr<Material> fontMaterial = RenderingFactory::Instance()->LoadMaterial("GUI/text");
					ConstructSettings::Texture2D settings;
					cgc::strong_ptr<Texture2D> texture = Texture2D::Construct(settings, image);
					fontMaterial->SetTexture2D(CT_HASH("tex"), texture);

					// clean up
					free(font.userdata);

					// create Font object
					return globalFonts.emplace(pathHash, std::string(p), fontMaterial, std::move(characters), fontInfo);
				}

			}
		}

		Debug::LogError("Could not load font: " + std::string(path));
		return cgc::strong_ptr<Font>();
	}
}