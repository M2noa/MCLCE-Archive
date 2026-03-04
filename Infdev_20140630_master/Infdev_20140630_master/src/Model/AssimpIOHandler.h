#pragma once

#include <assimp/IOSystem.hpp>

namespace Esteem
{
	class AssimpIOHandler : public Assimp::IOSystem
	{
	public:
		AssimpIOHandler();

		bool Exists(const char* pFile) const override;

		Assimp::IOStream* Open(const char* pFile, const char* pMode = "rb") override;

		void Close(Assimp::IOStream* pFile) override;

		char getOsSeparator() const override;
	};
}