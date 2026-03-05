#pragma once

#include <assimp/IOStream.hpp>
#include <cppu/cgc/pointers.h>

#include "Utils/File.h"
#include "Utils/Debug.h"

namespace Esteem
{
	class AssimpIOHandler;

	class AssimpIOStream : public Assimp::IOStream
	{
		friend class AssimpIOHandler;
	private:
		File file;

	protected:
		AssimpIOStream(File&& file)
			: file(std::move(file))
		{ }

	public:
		virtual void Flush()
		{
			//stream->
		}

		virtual size_t FileSize() const
		{
			return file.GetSize();
		}

		virtual size_t Read(void* pvBuffer, size_t pSize, size_t pCount)
		{
			file.GetStream()->read(reinterpret_cast<char*>(pvBuffer), pCount * pSize);
			return (size_t)file.GetStream()->gcount();
		}

		virtual aiReturn Seek(size_t pOffset, aiOrigin pOrigin)
		{
			switch (pOrigin)
			{
			case aiOrigin::aiOrigin_SET:
				file.GetStream()->seekg(pOffset, std::ios::beg);
				return aiReturn::aiReturn_SUCCESS;
			case aiOrigin::aiOrigin_CUR:
				file.GetStream()->seekg(pOffset, std::ios::cur);
				return aiReturn::aiReturn_SUCCESS;
			case aiOrigin::aiOrigin_END:
				file.GetStream()->seekg(pOffset, std::ios::end);
				return aiReturn::aiReturn_SUCCESS;
			}

			return aiReturn::aiReturn_FAILURE;
		}

		virtual size_t Tell()	const
		{
			return (size_t)file.GetStream()->tellg();
		}
		
		virtual size_t Write(const void* pvBuffer, size_t pSize, size_t pCount)
		{
			return AI_FAILURE;
		}
	};
}