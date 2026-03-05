#include "Material.h"

#include "Rendering/Objects/Buffer.h"
#include "Rendering/Objects/Texture1D.h"
#include "Rendering/Objects/Texture2D.h"
#include "Rendering/Objects/Texture3D.h"

#include "Rendering/RenderingFactory.h"

namespace Esteem
{
	Material::Material(std::string_view path, std::string_view name)
		: path(path)
		, name(name)
		, transparent(false)
		, reflective(false)
		, doubleSided(false)
	{
	}

	Material::~Material()
	{

	}
	
	cgc::strong_ptr<Material> Material::Construct(const std::string& path, TEXTURE_FILTER textureFlter)
	{
		return RenderingFactory::Instance()->LoadMaterial(path, textureFlter);
	}

	void Material::Bind()
	{

	}

	void Material::SetBuffer(hash_t bindHash, const cgc::strong_ptr<Buffer>& buffer)
	{
		buffers.SetBuffer(bindHash, buffer);
	}

	void Material::SetTexture1D(hash_t bindHash, const cgc::strong_ptr<Texture1D>& texture)
	{
		buffers.SetTexture1D(bindHash, texture);
	}

	void Material::SetTexture2D(hash_t bindHash, const cgc::strong_ptr<Texture2D>& texture)
	{
		buffers.SetTexture2D(bindHash, texture);
	}

	void Material::Buffers::SetBuffer(hash_t bindHash, const cgc::strong_ptr<Buffer>& buffer)
	{
		std::size_t i = 0;
		for (; i < bufferSize; ++i)
		{
			if (data[i].hash == bindHash)
			{
				data[i].buffer = buffer;
				return;
			}
		}

		data.insert(data.begin() + bufferSize, bindHash, buffer); // Evaluate $test buffers.data._data,2
		//buffers.data.emplace(buffers.data.begin() + buffers.bufferSize, bindHash, buffer);
		bufferSize++;
		texture1DSize++;
		texture2DSize++;
	}

	void Material::Buffers::SetTexture1D(hash_t bindHash, const cgc::strong_ptr<Texture1D>& texture)
	{
		std::size_t i = bufferSize; // all texture1Ds start where buffers ends
		for (; i < texture1DSize; ++i)
		{
			if (data[i].hash == bindHash)
			{
				data[i].buffer = texture;
				return;
			}
		}

		data.insert(data.begin() + texture1DSize, bindHash, texture);
		//buffers.data.emplace(buffers.data.begin() + buffers.texture1DSize, bindHash, texture);
		texture1DSize++;
		texture2DSize++;
	}

	void Material::Buffers::SetTexture2D(hash_t bindHash, const cgc::strong_ptr<Texture2D>& texture)
	{
		std::size_t i = texture1DSize; // all texture2Ds start where texture1Ds ends
		for (; i < texture2DSize; ++i)
		{
			if (data[i].hash == bindHash)
			{
				data[i].buffer = texture;
				return;
			}
		}

		data.insert(data.begin() + texture2DSize, bindHash, texture);
		//buffers.data.emplace(buffers.data.begin() + buffers.texture2DSize, bindHash, texture);
		texture2DSize++;
	}
}