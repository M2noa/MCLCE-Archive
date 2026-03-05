#include "Texture2D.h"
#include "Rendering/RenderingFactory.h"
#include "./Image.h"
#include "Utils/Debug.h"

namespace Esteem
{
	Texture2D::Texture2D(const Settings& settings)
		: AbstractBuffer(0)
		, settings(settings)
	{ }

	Texture2D::Texture2D(const Settings& settings, uint id, const std::string& path, const cgc::strong_ptr<Image>& image)
		: AbstractBuffer(id)
		, path(path)
		, image(image)
		, settings(settings)
	{ }

	Texture2D::~Texture2D()
	{
	}

	cgc::strong_ptr<Texture2D> Texture2D::Construct(const ConstructSettings::Texture2D& settings, const cgc::strong_ptr<Image>& image)
	{
		return RenderingFactory::Instance()->LoadTexture2D(settings, image);
	}

	cgc::strong_ptr<Texture2D> Texture2D::Construct(const ConstructSettings::Texture2D& settings, const std::string& name)
	{
		return RenderingFactory::Instance()->LoadTexture2D(settings, name);
	}

	cgc::strong_ptr<Texture2D> Texture2D::GetTexture(const std::string& name)
	{
		return RenderingFactory::Instance()->GetTexture2D(name);
	}

	void Texture2D::UpdateImage(const cgc::strong_ptr<Texture2D>& texture, const cgc::strong_ptr<Image>& image, const glm::uvec2& position)
	{
		RenderingFactory::Instance()->UpdateTexture2D(texture, image, position);
	}
}