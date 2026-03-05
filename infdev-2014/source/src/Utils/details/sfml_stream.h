#pragma once

#include "Utils/File.h"
#include <SFML/System/InputStream.hpp>

namespace details
{
	class sfml_stream : public sf::InputStream
	{
	private:
		Esteem::File file;

	public:
		sfml_stream(Esteem::File&& file)
			: file(std::move(file))
		{

		}

		sfml_stream()
		{

		}

		virtual ~sfml_stream()
		{

		}

		sf::Int64 read(void* data, sf::Int64 size) override
		{
			file.GetStream()->read(static_cast<char*>(data), size);
			return size;
		}

		////////////////////////////////////////////////////////////
		/// \brief Change the current reading position
		///
		/// \param position The position to seek to, from the beginning
		///
		/// \return The position actually sought to, or -1 on error
		///
		////////////////////////////////////////////////////////////
		sf::Int64 seek(sf::Int64 position) override
		{
			file.GetStream()->seekg(position);
			int64 pos = file.GetStream()->tellg();
			return pos;
		}

		////////////////////////////////////////////////////////////
		/// \brief Get the current reading position in the stream
		///
		/// \return The current position, or -1 on error.
		///
		////////////////////////////////////////////////////////////
		sf::Int64 tell() override
		{
			return file.GetStream()->tellg();
		}

		////////////////////////////////////////////////////////////
		/// \brief Return the size of the stream
		///
		/// \return The total number of bytes available in the stream, or -1 on error
		///
		////////////////////////////////////////////////////////////
		sf::Int64 getSize() override
		{
			return file.GetSize();
		};
	};
}