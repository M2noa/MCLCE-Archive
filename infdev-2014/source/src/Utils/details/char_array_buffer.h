#pragma once

#include <streambuf>

namespace Esteem
{
	namespace details
	{
		class char_array_buffer : public std::streambuf
		{
		private:
			const char* const begin;
			const char* const end;
			const char* current;

			int_type underflow();
			int_type uflow();
			int_type pbackfail(int_type ch);
			std::streamsize showmanyc();

		public:
			char_array_buffer(const char* data, unsigned int len);
		};

		char_array_buffer::char_array_buffer(const char* data, unsigned int len)
			: begin(data)
			, end(data + len)
			, current(data)
		{ }

		char_array_buffer::int_type char_array_buffer::underflow()
		{
			if (current == end)
				return traits_type::eof();

			return traits_type::to_int_type(*current);
		}

		char_array_buffer::int_type char_array_buffer::uflow()
		{
			if (current == end)
				return traits_type::eof();

			return traits_type::to_int_type(*current++);
		}

		char_array_buffer::int_type char_array_buffer::pbackfail(int_type ch)
		{
			if (current == begin || (ch != traits_type::eof() && ch != current[-1]))
				return traits_type::eof();

			return traits_type::to_int_type(*--current);
		}

		std::streamsize char_array_buffer::showmanyc()
		{
			return end - current;
		}
	}
}