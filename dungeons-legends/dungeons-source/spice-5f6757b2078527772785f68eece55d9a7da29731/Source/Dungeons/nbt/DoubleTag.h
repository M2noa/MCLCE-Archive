#pragma once

/* import java.io.* */
#include "common_header.h"

#include "nbt/Tag.h"

class DoubleTag : public Tag {

public:

	double data;

	DoubleTag(const std::string& name);

	DoubleTag(const std::string& name, double data);

	void write(IDataOutput& dos) const override /*throws IOException*/;

	void load(IDataInput& dis) override /*throws IOException*/;

	Type getId() const override;

	std::string toString() const override;

	//@Override
	Unique<Tag> copy() const override;

	//@Override
	bool equals(const Tag& rhs) const override;

};
