#pragma once

#include <RmlUi/Core/FileInterface.h>

namespace Esteem
{
	class HTMLFileInterface : public Rml::FileInterface
	{
		// Opens a file.
		virtual Rml::FileHandle Open(const Rml::String& path);

		// Closes a previously opened file.
		virtual void Close(Rml::FileHandle file);

		// Reads data from a previously opened file.
		virtual size_t Read(void* buffer, size_t size, Rml::FileHandle file);

		// Seeks to a point in a previously opened file.
		virtual bool Seek(Rml::FileHandle file, long offset, int origin);

		// Returns the current position of the file pointer.
		virtual size_t Tell(Rml::FileHandle file);
	};
}