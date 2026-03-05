#include "Dungeons.h"

#include "ByteTag.h"

ByteTag::ByteTag(const std::string& name) :
	Tag(name)
	, data(0){

}

ByteTag::ByteTag(const std::string& name, uint8_t data) :
	Tag(name)
	, data(data){

}

void ByteTag::write(IDataOutput& dos) const {
	dos.writeByte(data);
}

void ByteTag::load(IDataInput& dis) {
	data = dis.readByte();
}

Tag::Type ByteTag::getId() const {
	return Type::Byte;
}

std::string ByteTag::toString() const {
	return std::string(data, 1);
}

bool ByteTag::equals(const Tag& rhs) const {
	if (Tag::equals(rhs)) {
		return data == ((ByteTag&)rhs).data;
	}
	return false;
}

Unique<Tag> ByteTag::copy() const {
	return make_unique<ByteTag>(getName(), data);
}
