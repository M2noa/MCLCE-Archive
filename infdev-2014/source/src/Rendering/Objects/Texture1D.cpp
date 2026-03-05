#include "Texture1D.h"
#include "Rendering/RenderingFactory.h"
#include "./Image.h"

namespace Esteem
{
	Texture1D::Texture1D(const Settings& settings)
		: AbstractBuffer(0)
		, settings(settings)
	{ }

	Texture1D::Texture1D(const Settings& settings, uint id, const std::string& path, const cgc::strong_ptr<Image>& image)
		: AbstractBuffer(id)
		, path(path)
		, image(image)
		, settings(settings)
	{ }

	cgc::strong_ptr<Texture1D> Texture1D::Construct(const ConstructSettings::Texture1D& settings, const cgc::strong_ptr<Image>& image)
	{
		return RenderingFactory::Instance()->LoadTexture1D(settings, image);
	}

	cgc::strong_ptr<Texture1D> Texture1D::Construct(const ConstructSettings::Texture1D& settings, const std::string& name)
	{
		return RenderingFactory::Instance()->LoadTexture1D(settings, name);
	}

	void Texture1D::UpdateImage(const cgc::strong_ptr<Texture1D>& texture, const cgc::strong_ptr<Image>& image, uint offset)
	{
		RenderingFactory::Instance()->UpdateTexture1D(texture, image, offset);
	}
}