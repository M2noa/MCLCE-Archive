#pragma once

#include <string>
#include "stdafx.h"

namespace Esteem
{
	class IVBO
	{
	public:
		enum class Mapping
		{
			INTEGER = 0,
			FLOAT = 1,
			DOUBLE = 2,

			NORMALIZED = 0b100,

			FLOAT_NORMALIZED = FLOAT | NORMALIZED,
			DOUBLE_NORMALIZED = DOUBLE | NORMALIZED
		};

		virtual ~IVBO() = 0;
		virtual uint GetID() const = 0;

		virtual void AddVertexAttribute(std::string attribName, uint size, uint type, uint stride, void* offset, Mapping mapping = IVBO::Mapping::FLOAT) = 0;
		virtual void RemoveVertexAttribute(std::string attribName) = 0;
	};

	inline IVBO::~IVBO() {}
}