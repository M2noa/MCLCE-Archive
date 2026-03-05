/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#include "Dungeons.h"

#include "util/VariantParameterList.h"

bool VariantParameterList::hasParameter(const std::string& name) const {
	return parameters.find(name) != parameters.end();
}

ClassID::IDType VariantParameterList::getParameterType(const std::string& name) const {
	auto iter = parameters.find(name);
	if (iter == parameters.end()) {
		return 0;
	}

	return iter->second.type;
}

size_t VariantParameterList::getParameterCount() const {
	return parameters.size();
}
