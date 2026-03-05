#pragma once

#include "stdafx.h"
#include <cppu/cgc/pointers.h>

namespace Esteem
{
	class RenderingFactory;

	class AbstractBuffer
	{
		friend class RenderingFactory;
		friend class cgc::constructor;
	protected:
		uint id;

		AbstractBuffer(uint id)
			: id(id)
		{
		}

	public:
		inline uint GetID() const { return id; }
	};
}