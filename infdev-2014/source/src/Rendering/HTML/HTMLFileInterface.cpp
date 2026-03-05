#include "HTMLFileInterface.h"

#include "Utils/Data.h"
#include "Utils/File.h"

namespace Esteem
{
	// Opens a file.
	Rml::FileHandle HTMLFileInterface::Open(const Rml::String& path)
	{
		if (Data::AssetExists(path))
		{
			File* file = new File();
			*file = File::Open(path);
			return (Rml::FileHandle)file;
		}

		return (Rml::FileHandle)nullptr;
	}

	// Closes a previously opened file.
	void HTMLFileInterface::Close(Rml::FileHandle file)
	{
		delete (File*)file;
	}

	// Reads data from a previously opened file.
	size_t HTMLFileInterface::Read(void* buffer, size_t size, Rml::FileHandle file)
	{
		File* fFile = (File*)file;
		fFile->GetStream()->read(static_cast<char*>(buffer), size);

		return (size_t)fFile->GetStream()->gcount();
	}

	// Seeks to a point in a previously opened file.
	bool HTMLFileInterface::Seek(Rml::FileHandle file, long offset, int origin)
	{
		File* fFile = (File*)file;

		if constexpr (std::ios::beg == SEEK_SET && std::ios::cur == SEEK_CUR && std::ios::end == SEEK_END)
			fFile->GetStream()->seekg(offset, origin);
		else
		{
			int seekAnchor;
			switch (origin)
			{
			case SEEK_SET:
				seekAnchor = std::ios::beg;
				break;
			case SEEK_CUR:
				seekAnchor = std::ios::cur;
				break;
			case SEEK_END:
				seekAnchor = std::ios::end;
				break;
			}

			fFile->GetStream()->seekg(offset, seekAnchor);
		}


		return fFile->GetStream()->good();
	}

	// Returns the current position of the file pointer.
	size_t HTMLFileInterface::Tell(Rml::FileHandle file)
	{
		return (size_t)reinterpret_cast<File*>(file)->GetStream()->tellg();
	}
}