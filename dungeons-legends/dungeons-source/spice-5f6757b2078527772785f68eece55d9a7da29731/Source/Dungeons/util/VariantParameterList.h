/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "util/ClassID.h"

#include <string>
#include <unordered_map>

class VariantParameterList {
public:
	bool hasParameter(const std::string& name) const;

	ClassID::IDType getParameterType(const std::string& name) const;

	size_t getParameterCount() const;

	template<typename T>
	void pushParameter(const std::string& name, T* data) {
		Parameter p;
		p.type = ClassID::getID<T>();
		p.data = data;
		parameters[name] = p;
	}

	template<typename T>
	T* getParameter(const std::string& name) const {
		if (!hasParameter(name)) {
			return nullptr;
		}
		return static_cast<T*>(parameters.find(name)->second.data);
	}

	inline void clear() {
		parameters.clear();
	}

private:
	struct Parameter {
		ClassID::IDType type;
		void* data;
	};

	using ParameterMap = std::unordered_map<std::string, Parameter>;

	ParameterMap parameters;
};
