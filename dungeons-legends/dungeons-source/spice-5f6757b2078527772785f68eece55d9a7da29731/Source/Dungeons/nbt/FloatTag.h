#pragma once

/* import java.io.* */
#include "common_header.h"

#include "nbt/Tag.h"

class FloatTag : public Tag {

public:

	float data;

	FloatTag(const std::string& name);

	FloatTag(const std::string& name, float data);

	void write(IDataOutput& dos) const override /*throws IOException*/;

	void load(IDataInput& dis) override /*throws IOException*/;

	Type getId() const override;

	std::string toString() const override;

	//@Override
	Unique<Tag> copy() const override;

	//@Override
	bool equals(const Tag& rhs) const override;

};
