#include "TextureCube.h"

namespace Esteem
{
	TextureCube::TextureCube(const Settings& settings, uint id, const std::string& name)
		: AbstractBuffer(id)
		, name(name)
		, settings(settings)
	{ }

	TextureCube::TextureCube(const Settings& settings, uint id, const std::string& name, const std::vector<cgc::strong_ptr<Image>>& images)
		: AbstractBuffer(id)
		, settings(settings)
		, name(name)
		, images(images)
	{ }
}