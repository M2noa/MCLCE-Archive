#include "./Text.h"

namespace Esteem
{
	Text::Text(std::string text, const cgc::strong_ptr<Font>& font)
		: GraphicalOverlay::OverlayGraphic(font ? font->GetMaterial() : cgc::strong_ptr<Material>())
		, font(font)
		, text(text)
		, fontScale(1.f)
		, alignment(TEXT_ALIGNMENT::LEFT_TOP)
	{
		if (font)
			fontSettings = font->GetFontInfo();
	}

	void Text::Rebuild()
	{
		cgc::strong_ptr<Mesh<OverlayVertexData>::Data> meshData = cgc::gcnew<Mesh<OverlayVertexData>::Data>();
		AddToMesh(meshData->vertices, meshData->indices);

		if (!renderObject)
		{
			cgc::strong_ptr<Mesh<OverlayVertexData>> mesh = cgc::gcnew<Mesh<OverlayVertexData>>(meshData, false);
			renderObject = RenderObject::Construct(mesh, meshData, material);
		}
		else
			renderObject->GetMesh()->UpdateMeshData(renderObject->GetMesh(), meshData);
	}

	void Text::SetFont(const cgc::strong_ptr<Font>& font)
	{
		this->font = font;
		if (this->font)
		{
			material = font->GetMaterial();
			SetFontSize(fontSettings.fontSize);
		}

		Rebuild();
	}

	void Text::SetFontSize(float size)
	{
		if (font)
			fontSettings = font->GetFontInfo();

		fontScale = size / fontSettings.fontSize;
		fontSettings.fontSize = size;

		fontSettings.ascent *= fontScale;
		fontSettings.descent *= fontScale;
		fontSettings.fontSize *= fontScale;
		fontSettings.lineGap *= fontScale;
		fontSettings.lineHeight = round(fontSettings.lineHeight * fontScale);
		fontSettings.spaceWidth *= fontScale;

		Rebuild();
	}

	void Text::SetText(const std::string& text)
	{
		this->text = text;

		//visible = !text.empty();
		renderObject->SetInstanceCount(!text.empty());

		Rebuild();
	}

	bool Text::AddToMesh(std::vector<OverlayVertexData>& vertices, std::vector<uint>& indices)
	{
		if (font && !text.empty())
		{
			vertices.reserve(text.size() * 4);
			indices.reserve(text.size() * 6);

			glm::vec2 position(0, fontSettings.ascent);
			
			auto characters = font->GetFontCharacterInfo();
			for (uint i = 0; i < text.size(); ++i)
			{
				unsigned char c = text[i];
				if (c == ' ') // space
					position.x += fontSettings.spaceWidth;
				else if (c == '\n') // we ignore '\r', weird Windows >.>
				{
					position.y += fontSettings.lineHeight;
					areaSize.x = std::max(areaSize.x, position.x);
					position.x = 0;
				}
				else
				{
					c -= 33;
					if (c < characters.size())
					{
						auto character = characters[c];
						glm::vec2 offset(position + character.offset * fontScale);

						std::size_t index = vertices.size();

						vertices.emplace_back(glm::vec4(offset, 0, 1),
							color, glm::vec3(1), glm::vec2(character.uvData.left, character.uvData.top));

						vertices.emplace_back(glm::vec4(offset + glm::vec2(0, character.size.y * fontScale), 0, 1),
							color, glm::vec3(1), glm::vec2(character.uvData.left, character.uvData.top + character.uvData.height));

						vertices.emplace_back(glm::vec4(offset + character.size * fontScale, 0, 1),
							color, glm::vec3(1), glm::vec2(character.uvData.left + character.uvData.width, character.uvData.top + character.uvData.height));

						vertices.emplace_back(glm::vec4(offset + glm::vec2(character.size.x * fontScale, 0), 0, 1),
							color, glm::vec3(1), glm::vec2(character.uvData.left + character.uvData.width, character.uvData.top));

						indices.emplace_back(index);
						indices.emplace_back(index + 1);
						indices.emplace_back(index + 2);
						indices.emplace_back(index);
						indices.emplace_back(index + 2);
						indices.emplace_back(index + 3);

						position.x += round(character.advance * fontScale);
					}
				}
			}

			areaSize.y = position.y;
			areaSize.x = std::max(areaSize.x, position.x);

			// align
			position.y += fontSettings.lineHeight;
			position.x *= scale.x;
			position.y *= scale.y;
			switch (alignment)
			{
			case TEXT_ALIGNMENT::LEFT_MIDDLE:
				position.y *= 0.5f;
				// don't break, continue to LEFT_BOTTOM calculations
			case TEXT_ALIGNMENT::LEFT_BOTTOM:
				for (std::size_t i = 0; i < vertices.size(); ++i)
					vertices[i].position = matrix * (vertices[i].position + glm::vec4(0, -position.y, 0, 0));
				break;
			default:
				for (std::size_t i = 0; i < vertices.size(); ++i)
					vertices[i].position = matrix * vertices[i].position;
				break;
			}
		}

		return true;
	}
}