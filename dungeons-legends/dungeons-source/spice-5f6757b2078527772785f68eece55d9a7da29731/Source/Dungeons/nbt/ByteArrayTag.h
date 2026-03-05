#pragma once

/* import java.io.* */
#include "common_header.h"

#include "nbt/Tag.h"
#include "TagMemoryChunk.h"

class ByteArrayTag : public Tag {

public:

	TagMemoryChunk data;

	ByteArrayTag(const std::string& name);

	ByteArrayTag(const std::string& name, TagMemoryChunk data);

	Type getId() const override;

	std::string toString() const override;

	//@Override
	bool equals(const Tag& rhs) const override;

	//@Override
	Unique<Tag> copy() const override;

	void write(IDataOutput& dos) const override /*throws IOException*/;

	void load(IDataInput& dis) override /*throws IOException*/;

};
