#pragma once

#include <string>
#include "General/IFactory.h"
#include "./AbstractBSPFactory.h"
#include "./Objects/BSPMap.h"

namespace Esteem
{
	class BSPFactory : public AbstractBSPFactory
	{
	public:
		BSPMap* LoadBSP(std::string path);

	};
}