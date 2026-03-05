#include "Dungeons.h"

#include "Int64Tag.h"

Int64Tag::Int64Tag(const std::string& name) :
	Tag(name)
	, data(0){

}

Int64Tag::Int64Tag(const std::string& name, int64_t data) :
	Tag(name)
	, data(data){

}

void Int64Tag::write(IDataOutput& dos) const/*throws IOException*/
{
	dos.writeLongLong(data);
}

void Int64Tag::load(IDataInput& dis){	/*throws IOException*/
	data = dis.readLongLong();
}

Tag::Type Int64Tag::getId() const {
	return Type::Int64;
}

std::string Int64Tag::toString() const {
	std::stringstream ss;
	ss << data;
	return ss.str();
}

Unique<Tag> Int64Tag::copy() const {
	return make_unique<Int64Tag>(getName(), data);
}

bool Int64Tag::equals(const Tag& rhs) const {
	if (Tag::equals(rhs)) {
		return data == ((Int64Tag&)rhs).data;
	}
	return false;
}
