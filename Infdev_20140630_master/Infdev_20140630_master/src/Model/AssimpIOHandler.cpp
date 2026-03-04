#include "AssimpIOHandler.h"

#include "./AssimpIOStream.h"
#include "Utils/Data.h"
#include "Utils/File.h"

namespace Esteem
{
	AssimpIOHandler::AssimpIOHandler()
	{ }

	bool AssimpIOHandler::Exists(const char* pFile) const
	{
		return Data::AssetExists(pFile);
	}

	Assimp::IOStream* AssimpIOHandler::Open(const char* pFile, const char* pMode)
	{
		File file = File::Open(pFile);
		if (file.GetSize())
			return new AssimpIOStream(std::move(file));

		return nullptr;
	}

	void AssimpIOHandler::Close(Assimp::IOStream* pFile)
	{
		delete static_cast<AssimpIOStream*>(pFile);
	}

	char AssimpIOHandler::getOsSeparator() const
	{
		return '/';
	}
}