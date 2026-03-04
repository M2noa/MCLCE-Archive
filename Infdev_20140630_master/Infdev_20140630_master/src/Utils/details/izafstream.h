#pragma once

#include <istream>
#include "zip_archive_file_buffer.h"

class izafstream : public std::istream
{
private:
	details::zip_archive_file_buffer buffer;

public:
	izafstream(void* zipFileHandle)
		: buffer(zipFileHandle)
		, std::istream(&buffer)
	{

	}
	
	std::size_t get_pos() const
	{
		return buffer.get_pos();
	}

	std::size_t get_uncompressed_size() const
	{
		return buffer.get_uncompressed_size();
	}
};