#pragma once

#include "common_header.h"

#include "nbt/Tag.h"

class StringTag : public Tag {

public:

	std::string data;

	StringTag(const std::string& name);

	StringTag(const std::string& name, const std::string& data);

	void write(IDataOutput& dos) const override /*throws IOException*/;

	void load(IDataInput& dis) override /*throws IOException*/;

	Type getId() const override;

	std::string toString() const override;

	//@Override
	Unique<Tag> copy() const override;

	//@Override
	bool equals(const Tag& rhs) const override;

};
