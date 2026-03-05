#include "File.h"

#include "Utils/Data.h"

namespace Esteem
{
	File::File()
		: size(0)
		, writable(false)
	{
	}
	
	File::File(cgc::strong_ptr<std::istream> stream, size_t size)
		: stream(std::move(stream))
		, size(size)
		, writable(false)
	{
	}

	File::File(const File& copy)
		: stream(copy.stream)
		, size(copy.size)
		, writable(copy.writable)
	{

	}

	File::File(File&& move)
		: stream(std::move(move.stream))
		, size(move.size)
		, writable(move.writable)
	{
		move.size = 0;
		move.writable = 0;
	}

	File& File::operator=(const File& copy)
	{
		stream = copy.stream;
		size = copy.size;
		writable = copy.writable;

		return *this;
	}

	File& File::operator=(File&& move) noexcept
	{
		stream = std::move(move.stream);
		size = move.size;
		writable = move.writable;
		
		move.size = 0;
		move.writable = 0;

		return *this;
	}

	File::~File()
	{
	}

	File File::Open(const std::string& path)
	{
		size_t size;
		auto stream = Data::StreamAsset(path, size);
		return File(stream, size);
	}

	std::size_t File::GetSize() const
	{
		return size;
	}

	std::istream* File::GetStream() const
	{
		return stream.ptr();
	}

	std::string File::GetAllData() const
	{
		std::string contents;

		if (stream)
		{
			contents.resize(size);
			stream->read(contents.data(), size);
		}

		return contents;
	}
}