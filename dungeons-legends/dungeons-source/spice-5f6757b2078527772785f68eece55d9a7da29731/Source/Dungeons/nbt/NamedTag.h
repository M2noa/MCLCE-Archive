#pragma once

#include "common_header.h"
#include "Tag.h"

class NamedTag : public Tag {
public:

	NamedTag(const std::string& name) :
		Tag(name)
		, name(name) {

	}

	virtual Tag* setName(const std::string& name_) override {
		name = name_;
		return this;
	}

	virtual std::string getName() const override {
		return name;
	}

protected:

	std::string name;
};
