#pragma once

/* import java.io.* */
#include "common_header.h"

#include "nbt/Tag.h"

class ByteTag : public Tag {

public:

	uint8_t data;

	ByteTag(const std::string& name);

	ByteTag(const std::string& name, uint8_t data);

	void write(IDataOutput& dos) const override /*throws IOException*/;

	void load(IDataInput& dis) override /*throws IOException*/;

	Type getId() const override;

	std::string toString() const override;

	//@Override
	bool equals(const Tag& rhs) const override;

	//@Override
	Unique<Tag> copy() const override;

};
