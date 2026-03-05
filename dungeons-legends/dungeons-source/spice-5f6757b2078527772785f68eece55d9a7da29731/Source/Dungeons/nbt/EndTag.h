#pragma once

#include "Tag.h"

/* import java.io.* */

class EndTag : public Tag {

public:

	EndTag()
		: Tag(""){
	}

	void load(IDataInput& dis)  override {/*throws IOException*/
	}

	void write(IDataOutput& dos) const  override /*throws IOException*/ {
	}

	Type getId() const override {
		return Type::End;
	}

	std::string toString() const override {
		return "END";
	}

	//@Override
	Unique<Tag> copy() const override {
		return make_unique<EndTag>();
	}

	//@Override
	bool equals(const Tag& rhs) const override {
		return Tag::equals(rhs);
	}

};
