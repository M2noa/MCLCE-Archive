#include "Buffer.h"
#include "Rendering/RenderingFactory.h"

namespace Esteem
{
	Buffer::Buffer(const Buffer::Settings& settings)
		: AbstractBuffer(0)
		, bufferId(0)
		, settings(settings)
	{ }

	Buffer::Buffer(const Buffer::Settings& settings, uint textureId, uint bufferId, const std::string& name)
		: AbstractBuffer(textureId)
		, bufferId(bufferId)
		, name(name)
		, settings(settings)
	{ }

	cgc::strong_ptr<Buffer> Buffer::Construct(const ConstructSettings::Buffer& settings, const std::string& name, void* data, std::size_t offset, std::size_t size)
	{
		return RenderingFactory::Instance()->LoadBuffer(settings, name, data, offset, size);
	}

	void Buffer::Update(const cgc::strong_ptr<Buffer>& buffer, const void* data, std::size_t offset, std::size_t size)
	{
		RenderingFactory::Instance()->UpdateBuffer(buffer, data, offset, size, true);
	}
}