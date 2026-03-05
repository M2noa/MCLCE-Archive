#pragma once

#include <istream>
#include <string>
#include <cppu/cgc/pointers.h>

namespace Esteem
{
	class File
	{
	private:
		cgc::strong_ptr<std::istream> stream;
		std::size_t size;
		bool writable;

		File(cgc::strong_ptr<std::istream> stream, size_t size);

	public:
		File();
		~File();

		File(const File& copy);
		File(File&& move);

		static File Open(const std::string& path);

		std::size_t GetSize() const;
		std::istream* GetStream() const;
		std::string GetAllData() const;

		File& operator=(const File& other);
		File& operator=(File&& other) noexcept;
	};
}