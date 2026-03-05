#pragma once

#include "common_header.h"

#include "nbt/Tag.h"
/* import java.io.* */

class IntTag : public Tag {

public:

	int data;

	IntTag(const std::string& name);

	IntTag(const std::string& name, int data);

	void write(IDataOutput& dos) const override /*throws IOException*/;

	void load(IDataInput& dis) override /*throws IOException*/;

	Type getId() const override;

	std::string toString() const override;

	//@Override
	Unique<Tag> copy() const override;

	//@Override
	bool equals(const Tag& rhs) const override;

};
