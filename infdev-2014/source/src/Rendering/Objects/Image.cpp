#include "./Image.h"

#include "Utils/File.h"
#include "Utils/Debug.h"
#define private public
#include <SFML/Graphics/Image.hpp>

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>


namespace Esteem
{
	void Image::Create(unsigned int width, unsigned int height, const Color8u& color)
	{
		stride = 4;
		std::size_t pixelSize = width * height;
		free(pixelBuffer);

		pixelBuffer = static_cast<unsigned char*>(malloc(pixelSize * stride));
		if (pixelBuffer)
		{
			for (std::size_t i = 0; i < pixelSize; i += stride)
				memcpy(pixelBuffer + i, &color.r, stride);
		}
	}

	////////////////////////////////////////////////////////////
	void Image::Create(unsigned int width, unsigned int height, uint8* pixels, bool takeOver, uint8 stride)
	{
		if(pixelBuffer != pixels)
			free(pixelBuffer);

		this->stride = stride;
		pixelBufferSize = width * height * stride;
		size.x = width;
		size.y = height;

		if (takeOver)
			pixelBuffer = pixels;
		else
		{
			pixelBuffer = static_cast<unsigned char*>(malloc(pixelBufferSize));
			if (pixelBuffer)
				memcpy(pixelBuffer, pixels, pixelBufferSize);
		}
	}
	
	bool Image::SetFilePath(const std::string& filename)
	{
		bool exists = Data::AssetExists(filename);
		if (exists)
		{
			path = filename;
			if (ownsBuffer)
				free(pixelBuffer);
			else
				pixelBuffer = nullptr;
			pixelBufferSize = 0;
		}

		return exists;
	}

	bool Image::LoadFileIfNotLoaded()
	{
		return pixelBuffer || (pixelBuffer == nullptr && !path.empty() && LoadFromFile(path));
	}

	////////////////////////////////////////////////////////////
	bool Image::LoadFromFile(const std::string& filename)
	{
		std::string contents = Data::ReadAsset(filename);
		bool success = LoadFromMemory(contents.data(), contents.size());

		if (success)
		{
			path = filename;
			return true;
		}

		return false;
	}


	////////////////////////////////////////////////////////////
	bool Image::LoadFromMemory(const void* data, std::size_t size, std::string_view name)
	{
		this->path = name;
		Vector2i imageSize;
		int imageChannels;
		int desiredChannels = STBI_rgb_alpha;

		pixelBuffer = stbi_load_from_memory(static_cast<const unsigned char*>(data), size, &imageSize.x, &imageSize.y, &imageChannels, desiredChannels);
		if (pixelBuffer)
		{
			stride = desiredChannels == STBI_default ? imageChannels : desiredChannels;
			pixelBufferSize = imageSize.x * imageSize.y * stride;
			this->size = Vector2u(imageSize);

			return true;
		}
		
		return false;
	}


	////////////////////////////////////////////////////////////
	/*bool Image::LoadFromStream(InputStream& stream)
	{
		return sf::priv::ImageLoader::getInstance().loadImageFromStream(stream, pixels, size);
	}*/


	////////////////////////////////////////////////////////////
	bool Image::SaveToFile(const std::string& filename) const
	{
		std::size_t dotPos = filename.rfind('.');
		std::string extension = filename.substr(dotPos + 1);

		int success = 0;

		if (extension == "png")
			success = stbi_write_png(filename.c_str(), size.x, size.y, stride, pixelBuffer, size.x * stride);
		else if (extension == "bmp")
			success = stbi_write_bmp(filename.c_str(), size.x, size.y, stride, pixelBuffer);
		else if (extension == "tga")
			success = stbi_write_tga(filename.c_str(), size.x, size.y, stride, pixelBuffer);
		else if (extension == "jpg" || extension == "jpeg")
			success = stbi_write_jpg(filename.c_str(), size.x, size.y, stride, pixelBuffer, 80);
		else
			Debug::LogError("Can't save image with the following extension: " + extension);
		
		return success;
	}

	////////////////////////////////////////////////////////////
	Vector2u Image::GetSize() const
	{
		return size;
	}
	
	////////////////////////////////////////////////////////////
	void Image::SetPixel(unsigned int x, unsigned int y, const Color8u& color)
	{
		if (pixelBuffer)
			memcpy(pixelBuffer + x * y * stride, &color.r, stride);
	}
	
	////////////////////////////////////////////////////////////
	Color8u Image::GetPixel(unsigned int x, unsigned int y) const
	{
		Color8u color;
		if (pixelBuffer)
			memcpy(&color.r, pixelBuffer + x * y * stride, stride);

		return color;
	}

	////////////////////////////////////////////////////////////
	const uint8* Image::GetPixelsPtr() const
	{
		return pixelBuffer;
	}

	////////////////////////////////////////////////////////////
	/*void Image::FlipHorizontally()
	{
		
	}*/

	////////////////////////////////////////////////////////////
	void Image::FlipVertically()
	{
		size_t bytes_per_row = (size_t)size.x * stride;
		stbi_uc temp[2048];
		stbi_uc* bytes = (stbi_uc*)pixelBuffer;

		for (uint row = 0; row < (size.y >> 1); ++row)
		{
			stbi_uc* row0 = bytes + row * bytes_per_row;
			stbi_uc* row1 = bytes + (size.y - row - 1) * bytes_per_row;

			// swap row0 with row1
			size_t bytes_left = bytes_per_row;
			while (bytes_left)
			{
				size_t bytes_copy = (bytes_left < sizeof(temp)) ? bytes_left : sizeof(temp);
				memcpy(temp, row0, bytes_copy);
				memcpy(row0, row1, bytes_copy);
				memcpy(row1, temp, bytes_copy);
				row0 += bytes_copy;
				row1 += bytes_copy;
				bytes_left -= bytes_copy;
			}
		}
	}
}
#undef private