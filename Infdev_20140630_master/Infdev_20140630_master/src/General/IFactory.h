#pragma once

#include <string>

namespace Esteem
{
	class IFactory
	{
	public:
		virtual ~IFactory() = 0;
		
		virtual void Initialize() = 0;

		/// \brief used by the Data class to enable factory search, put in own logic in here
		virtual bool EqualsSearch(std::string_view search) = 0;
	};

	inline IFactory::~IFactory() {}
}