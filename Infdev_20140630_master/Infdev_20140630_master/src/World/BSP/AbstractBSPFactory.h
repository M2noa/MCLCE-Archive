#pragma once

#include "General/IFactory.h"
#include "./Objects/BSPMap.h"

namespace Esteem
{
	class AbstractBSPFactory : public IFactory
	{
	public:
		virtual BSPMap LoadBSP() = 0;
	};
}