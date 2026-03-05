#pragma once

#include "stdafx.h"
#include <string>

namespace Esteem
{
	class IUBO
	{
	public:
		virtual ~IUBO();

		virtual uint GetID() const = 0;
		virtual uint GetBindingIndex() const = 0;
		virtual const std::string& GetBindingName() const = 0;

		virtual void UpdateBuffer(const void* data, uint size) = 0;
	};

	inline IUBO::~IUBO() {}
}