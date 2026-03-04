#pragma once

#include <string>
#include "stdafx.h"

namespace Esteem
{
	class IEBO
	{
	public:
		virtual ~IEBO() = 0;
		virtual uint GetID() const = 0;
	};

	inline IEBO::~IEBO() {}
}