#include "Dungeons.h"

#include "DoubleTag.h"

DoubleTag::DoubleTag(const std::string& name) :
	Tag(name)
	, data(0.0){

}

DoubleTag::DoubleTag(const std::string& name, double data) :
	Tag(name)
	, data(data){

}

void DoubleTag::write(IDataOutput& dos) const	/*throws IOException*/
{
	dos.writeDouble(data);
}

void DoubleTag::load(IDataInput& dis){	/*throws IOException*/
	data = dis.readDouble();
}

Tag::Type DoubleTag::getId() const {
	return Type::Double;
}

std::string DoubleTag::toString() const {
	std::stringstream ss;
	ss << data;
	return ss.str();
}

Unique<Tag> DoubleTag::copy() const {
	return make_unique<DoubleTag>(getName(), data);
}

bool DoubleTag::equals(const Tag& rhs) const {
	if (Tag::equals(rhs)) {
		return data == ((DoubleTag&)rhs).data;
	}
	return false;
}
