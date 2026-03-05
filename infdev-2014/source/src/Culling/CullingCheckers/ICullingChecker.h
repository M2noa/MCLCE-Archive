#pragma once

#include "../../Rendering/Objects/RenderObject.h"

namespace Esteem
{
	class ICullingChecker
	{
	public:
		virtual ~ICullingChecker() = 0;

		virtual bool SingleObjectCheck(const RenderObject* object) = 0;
		virtual void RangeOfObjectsCheck() = 0;
	};

	inline ICullingChecker::~ICullingChecker() {}
}