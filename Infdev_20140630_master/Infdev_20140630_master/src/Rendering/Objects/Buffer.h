#pragma once

#include "stdafx.h"
#include <string>
#include <bitset>
#include <cppu/cgc/pointers.h>

#include "AbstractBuffer.h"
#include "General/Color.h"
#include "../RenderingFactory.h"
#include "../rtypes.h"

namespace Esteem
{
	class Image;

	class Buffer : public AbstractBuffer
	{
		friend class RenderingFactory;
		friend class cgc::constructor;
	public:
		struct Settings
		{
			size_t size;
			TEXTURE_FORMAT format;
			std::bitset<8> flags;

			Settings(const ConstructSettings::Buffer& settings)
				: size(settings.size)
				, format(settings.format)
				, flags(uint8(settings.keepImageOnCPU))
			{ }

			bool KeepImageOnCPU() const { return flags[0]; }
		};
		
	private:
		uint bufferId;
		std::string name;
		cgc::strong_ptr<Image> image;
		Settings settings;

		Buffer(const Settings& settings);
		Buffer(const Settings& settings, uint textureId, uint bufferId, const std::string& path);

	public:
		static cgc::strong_ptr<Buffer> Construct(const ConstructSettings::Buffer& settings, const std::string& name, void* data, std::size_t offset, std::size_t size);

		inline uint GetBufferID() const { return bufferId; }
		inline const std::string& GetName() const { return name; }
		inline const Settings& GetSettings() const { return settings; }

		static void Update(const cgc::strong_ptr<Buffer>& buffer, const void* data, std::size_t offset, std::size_t size);

	};
}