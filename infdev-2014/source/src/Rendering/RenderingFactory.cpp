#include "RenderingFactory.h"
#include "Rendering/Objects/Buffer.h"
#include "Rendering/Objects/Texture1D.h"
#include "Rendering/Objects/Texture2D.h"
#include "Rendering/Objects/TextureCube.h"

namespace Esteem
{
	RenderingFactory* RenderingFactory::instance = nullptr;

	void RenderingFactory::AlterBuffer(const cgc::strong_ptr<Buffer>& buffer, uint textureId, uint bufferId, const ConstructSettings::Buffer* settings)
	{
		if (buffer)
		{
			buffer->id = textureId;
			buffer->bufferId = bufferId;
			if (settings)
				buffer->settings = *settings;
		}
	}

	void RenderingFactory::AlterTexture1D(const cgc::strong_ptr<Texture1D>& texture, uint id, const std::string& path, const ConstructSettings::Texture1D* settings, const cgc::strong_ptr<Image>& image)
	{
		if (texture)
		{
			texture->id = id;
			texture->image = image;
			texture->path = path;
			if (settings)
				texture->settings = *settings;
		}
	}

	void RenderingFactory::AlterTexture2D(const cgc::strong_ptr<Texture2D>& texture, uint id, const std::string& path, const ConstructSettings::Texture2D* settings, const cgc::strong_ptr<Image>& image)
	{
		if (texture)
		{
			texture->id = id;
			texture->image = image;
			texture->path = path;
			if (settings)
				texture->settings = *settings;
		}
	}

	void RenderingFactory::AlterTextureCube(const cgc::strong_ptr<TextureCube>& texture, uint id, const glm::uvec2& size)
	{
		if (texture)
		{
			texture->id = id;
			texture->size = size;
		}
	}
}