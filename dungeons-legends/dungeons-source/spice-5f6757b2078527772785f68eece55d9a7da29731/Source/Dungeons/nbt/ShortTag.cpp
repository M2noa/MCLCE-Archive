#include "Dungeons.h"

#include "ShortTag.h"

ShortTag::ShortTag(const std::string& name) :
	Tag(name)
	, data(0){

}

ShortTag::ShortTag(const std::string& name, short data) :
	Tag(name)
	, data(data){

}

void ShortTag::write(IDataOutput& dos) const/*throws IOException*/
{
	dos.writeShort(data);
}

void ShortTag::load(IDataInput& dis){	/*throws IOException*/
	data = dis.readShort();
}

Tag::Type ShortTag::getId() const {
	return Type::Short;
}

std::string ShortTag::toString() const {
	std::stringstream ss;
	ss << data;
	return ss.str();
}

Unique<Tag> ShortTag::copy() const {
	return make_unique<ShortTag>(getName(), data);
}

bool ShortTag::equals(const Tag& rhs) const {
	if (Tag::equals(rhs)) {
		return data == ((ShortTag&)rhs).data;
	}
	return false;
}
