#include "Dungeons.h"

#include "StringTag.h"

StringTag::StringTag(const std::string& name) :
	Tag(name){
}

StringTag::StringTag(const std::string& name, const std::string& data) :
	Tag(name)
	, data(data) {

}

void StringTag::write(IDataOutput& dos) const	/*throws IOException*/
{
	dos.writeString(data);
}

void StringTag::load(IDataInput& dis){	/*throws IOException*/
	data = dis.readString();// just a tiny bit slower, but safer
}

Tag::Type StringTag::getId() const {
	return Type::String;
}

std::string StringTag::toString() const {
	std::stringstream ss;
	ss << data;
	return ss.str();
}

Unique<Tag> StringTag::copy() const {
	return make_unique<StringTag>(getName(), data);
}

bool StringTag::equals(const Tag& rhs) const {
	if (Tag::equals(rhs)) {
		StringTag& o = (StringTag&)rhs;
		return data == o.data;
	}
	return false;
}
