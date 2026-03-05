#include "Dungeons.h"

#include "IntTag.h"

IntTag::IntTag(const std::string& name) :
	Tag(name)
	, data(0){

}

IntTag::IntTag(const std::string& name, int data) :
	Tag(name)
	, data(data){

}

void IntTag::write(IDataOutput& dos) const	/*throws IOException*/
{
	dos.writeInt(data);
}

void IntTag::load(IDataInput& dis){	/*throws IOException*/
	data = dis.readInt();
}

Tag::Type IntTag::getId() const {
	return Type::Int;
}

std::string IntTag::toString() const {
	std::stringstream ss;
	ss << data;
	return ss.str();
}

Unique<Tag> IntTag::copy() const {
	return make_unique<IntTag>(getName(), data);
}

bool IntTag::equals(const Tag& rhs) const {
	if (Tag::equals(rhs)) {
		return data == ((IntTag&)rhs).data;
	}
	return false;
}
