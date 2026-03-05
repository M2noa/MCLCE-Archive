#pragma once

#include <istream>
#include <streambuf>
#include <vector>
#include <zlib/zlib.h>
#include <cppu/cgc/pointers.h>

namespace details
{
	class zipfilebuffer : public std::streambuf
	{
	private:
		cgc::strong_ptr<std::istream> stream;
		std::size_t streamSize;
		z_stream zipStream;

		std::size_t position;
		unsigned char* cur;
		unsigned char* end;
		int_type curChar;

		std::vector<unsigned char> buffer;
		unsigned char* outputBuffer;
		std::size_t halfBufferSize;

		int_type underflow()
		{
			return curChar;
		}

		int_type uflow()
		{
			int_type lastChar = curChar;
			
			if (zipStream.avail_out == 0)
			{
				cur++;
				position++;

				if (cur == end)
				{
					if (ReadNextUntilFull() > 0)
						cur = outputBuffer;
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
			std::size_t bufferEnd = (size_t)stream->tellg();
			std::size_t bufferStart = bufferEnd - buffer.size();

			if (sp >= bufferStart && sp < bufferEnd) // we can move inside our buffered data
			{
				cur = buffer.data() + ((std::size_t)sp - bufferStart);
				curChar = *cur;
				position = (size_t)sp;
			}
			else
			{
				// need to restart from the beginning (this can become really slow)
				if (sp < bufferStart)
					stream->seekg(0);

				// move ahead
				do
				{
					if (ReadNextUntilFull() <= 0)
						return traits_type::eof();

					bufferStart += buffer.size();

				} while (buffer.size() + bufferStart < sp);

				cur = buffer.data() + ((std::size_t)sp - bufferStart);
				curChar = *cur;
				position = (size_t)sp;
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
				return 0;
				break;
			}

			if(pos != position)
				seekpos(pos, which);

			return position;
		}

		std::streamsize showmanyc()
		{
			return 0;
		}

		std::size_t ReadNextUntilFull()
		{
			zipStream.avail_out = halfBufferSize;
			zipStream.next_out = outputBuffer;

			while (zipStream.avail_out > 0 && stream->tellg() < streamSize);
			{
				if (zipStream.avail_in == 0)
				{
					stream->read(reinterpret_cast<char*>(buffer.data()), halfBufferSize);
					zipStream.avail_in = (uInt)stream->gcount();
					zipStream.next_in = buffer.data();
				}

				if (zipStream.avail_in == 0 || inflate(&zipStream, Z_NO_FLUSH) != Z_OK)
					return 0;
			}

			return halfBufferSize - zipStream.avail_out;
		}

	public:
		zipfilebuffer(const cgc::strong_ptr<std::istream>& stream, std::size_t streamSize, std::size_t bufferSize = 1024)
			: stream(stream)
			, streamSize(streamSize)
			, position(0)
			, curChar(0)
			, halfBufferSize(bufferSize / 2)
		{
			buffer.resize(bufferSize);
			outputBuffer = buffer.data() + halfBufferSize;

			cur = buffer.data();
			end = cur + buffer.size();
			curChar = *cur;

			zipStream.zalloc = Z_NULL;
			zipStream.zfree = Z_NULL;
			zipStream.opaque = Z_NULL;
			zipStream.avail_in = halfBufferSize;
			zipStream.next_in = buffer.data();
			zipStream.avail_out = halfBufferSize;
			zipStream.next_out = buffer.data() + buffer.size() / 2;

			inflateInit(&zipStream);
		}

		~zipfilebuffer()
		{
			
		}

		std::size_t get_pos() const
		{
			return position;
		}
	};
}