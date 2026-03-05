#include "Dungeons.h"

#include "FloatTag.h"

FloatTag::FloatTag(const std::string& name) :
	Tag(name)
	, data(0.0){

}

FloatTag::FloatTag(const std::string& name, float data) :
	Tag(name)
	, data(data){

}

void FloatTag::write(IDataOutput& dos) const/*throws IOException*/
{
	dos.writeFloat(data);
}

void FloatTag::load(IDataInput& dis){	/*throws IOException*/
	data = dis.readFloat();
}

Tag::Type FloatTag::getId() const {
	return Type::Float;
}

std::string FloatTag::toString() const {
	std::stringstream ss;
	ss << data;
	return ss.str();
}

Unique<Tag> FloatTag::copy() const {
	return make_unique<FloatTag>(getName(), data);
}

bool FloatTag::equals(const Tag& rhs) const {
	if (Tag::equals(rhs)) {
		return data == ((FloatTag&)rhs).data;
	}
	return false;
}
