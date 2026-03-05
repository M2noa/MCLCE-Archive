#pragma once
#include "common_header.h"

#include "nbt/Tag.h"

class Int64Tag : public Tag {

public:

	int64_t data;

	Int64Tag(const std::string& name);

	Int64Tag(const std::string& name, int64_t data);

	void write(IDataOutput& dos) const override /*throws IOException*/;

	void load(IDataInput& dis) override /*throws IOException*/;

	Type getId() const override;

	std::string toString() const override;

	//@Override
	Unique<Tag> copy() const override;

	//@Override
	bool equals(const Tag& rhs) const override;

};
