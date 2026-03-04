#pragma once

#include <streambuf>

namespace Esteem
{
	namespace details
	{
		class string_buffer : public std::streambuf
		{
		private:
			std::string str;
			unsigned int current;

			int_type underflow()
			{
				if (current == str.size())
					return traits_type::eof();

				return str[current];
			}

			int_type uflow()
			{
				if (current == str.size())
					return traits_type::eof();

				return str[current++];
			}

			int_type pbackfail(int_type ch)
			{
				if (current == 0 || (ch != traits_type::eof() && ch != str[current - 1]))
					return traits_type::eof();

				return str[--current];
			}

			inline std::streamsize showmanyc()
			{
				return str.size() - current;
			}

		public:
			string_buffer()
				: current(0)
			{ }

			inline std::string& getstring()
			{
				return str;
			}

			inline char* getpospointer()
			{
				return str.data() + current;
			}

			inline int_type getpos()
			{
				return current;
			}
		};
	}
}