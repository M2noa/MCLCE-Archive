#pragma once

#include <streambuf>
#include "minizip/unzip.h"

namespace details
{
	class zip_archive_file_buffer : public std::streambuf
	{
	private:
		void* zipFileHandle;
		unz_file_info_s fileInfo;
		std::size_t position;
		unsigned char* cur;
		unsigned char* end;
		int_type curChar;
		std::vector<unsigned char> buffer;

		int_type underflow()
		{
			return curChar;
		}

		int_type uflow()
		{
			int_type lastChar = curChar;

			if (position < fileInfo.uncompressed_size - 1)
			{
				cur++;
				position++;

				if (cur == end)
				{
					if (unzReadCurrentFile(zipFileHandle, buffer.data(), buffer.size()) > 0)
						cur = buffer.data();
					else
						curChar = traits_type::eof();
				}

				curChar = *cur;
			}
			else
				curChar = traits_type::eof();

			return lastChar;
		}

		int_type pbackfail(int_type ch)
		{
			if (cur == buffer.data() || (ch != traits_type::eof() && ch != cur[-1]))
				return traits_type::eof();

			return *--cur;
		}

		std::streampos seekpos(std::streampos sp, std::ios_base::openmode which)
		{
			std::size_t bufferEnd = unztell(zipFileHandle);
			std::size_t bufferStart = bufferEnd - buffer.size();

			if (sp >= bufferStart && sp < bufferEnd) // we can move inside our buffered data
			{
				cur = buffer.data() + ((std::size_t)sp - bufferStart);
				curChar = *cur;
				position = sp;
			}
			else
			{
				// need to restart from the beginning (this can become really slow)
				if (sp < bufferStart)
				{
					if (unzCloseCurrentFile(zipFileHandle) == UNZ_OK && unzOpenCurrentFile(zipFileHandle) == UNZ_OK)
					{
						if (unzReadCurrentFile(zipFileHandle, buffer.data(), buffer.size()) <= 0)
							return traits_type::eof();

						bufferStart = 0;
					}
					else
						return traits_type::eof();
				}

				// move ahead
				do
				{
					if (unzReadCurrentFile(zipFileHandle, buffer.data(), buffer.size()) <= 0)
						return traits_type::eof();

					bufferStart += buffer.size();

				} while (buffer.size() + bufferStart < sp);

				cur = buffer.data() + ((std::size_t)sp - bufferStart);
				curChar = *cur;
				position = sp;
			}

			return position;
		}

		std::streampos seekoff(std::streamoff off, std::ios_base::seekdir way, std::ios_base::openmode which)
		{
			std::streampos pos;

			switch (way)
			{
			case std::ios::beg:
				pos = off;
				break;
			case std::ios::cur:
				pos = position + off;
				break;
			case std::ios::end:
				pos = fileInfo.uncompressed_size + off;
				break;
			}

			if (pos == fileInfo.uncompressed_size)
				position = pos; // can't/shouldn't read here anyway
			else if(pos != position)
				seekpos(pos, which);

			return position;
		}

		std::streamsize showmanyc()
		{
			return fileInfo.uncompressed_size - position;
		}

	public:
		zip_archive_file_buffer(void* zipFileHandle, std::size_t bufferSize = 512)
			: zipFileHandle(zipFileHandle)
			, position(0)
			, curChar(0)
		{
			buffer.resize(bufferSize);
			cur = buffer.data();
			end = cur + buffer.size();
			unzGetCurrentFileInfo(zipFileHandle, &fileInfo, nullptr, 0, nullptr, 0, nullptr, 0);
			unzReadCurrentFile(zipFileHandle, buffer.data(), buffer.size());
			curChar = *cur;
		}

		~zip_archive_file_buffer()
		{
			unzCloseCurrentFile(zipFileHandle);
			unzClose(zipFileHandle);
		}

		zip_archive_file_buffer(zip_archive_file_buffer&& move)
			: zipFileHandle(move.zipFileHandle)
			, fileInfo(move.fileInfo)
			, position(move.position)
			, cur(move.cur)
			, end(move.end)
			, curChar(move.curChar)
		{
			buffer.swap(move.buffer);
			move.zipFileHandle = nullptr;
		}

		zip_archive_file_buffer& operator=(zip_archive_file_buffer&& move)
		{
			zipFileHandle = move.zipFileHandle;
			fileInfo = move.fileInfo;
			position = move.position;
			cur = move.cur;
			end = move.end;
			curChar = move.curChar;

			buffer.swap(move.buffer);
			move.zipFileHandle = nullptr;

			return *this;
		}

		std::size_t get_pos() const
		{
			return position;
		}

		std::size_t get_uncompressed_size() const
		{
			return fileInfo.uncompressed_size;
		}
	};
}