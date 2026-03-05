#pragma once

#include <string>
#include "./Font.h"
#include "Rendering/Objects/GraphicalOverlay.h"

namespace Esteem
{
	class Text : public GraphicalOverlay::OverlayGraphic
	{
	public:
		enum TEXT_ALIGNMENT
		{
			LEFT_TOP,
			LEFT_MIDDLE,
			LEFT_BOTTOM,

			CENTER_TOP,
			CENTER_MIDDLE,
			CENTER_BOTTOM,
			
			RIGHT_TOP,
			RIGHT_MIDDLE,
			RIGHT_BOTTOM
		};

	private:
		cgc::strong_ptr<Font> font;
		std::string text;
		details::FontInfo fontSettings;
		float fontScale;
		glm::vec2 areaSize;

		void Rebuild();

	public:
		TEXT_ALIGNMENT alignment;

		Text(std::string text = "", const cgc::strong_ptr<Font>& font = cgc::strong_ptr<Font>());

		void SetFont(const cgc::strong_ptr<Font>& font);
		inline const cgc::strong_ptr<Font>& GetFont() const { return font; }

		const glm::vec2& GetAreaSize() const { return areaSize; }

		void SetFontSize(float size);
		float GetFontSize() const { return fontSettings.fontSize; }

		const details::FontInfo& GetFontSettings() const { return fontSettings; }

		void SetText(const std::string& text);
		const std::string& GetText() const { return text; }


		bool AddToMesh(std::vector<OverlayVertexData>& vertices, std::vector<uint>& indices) override;
	};
}