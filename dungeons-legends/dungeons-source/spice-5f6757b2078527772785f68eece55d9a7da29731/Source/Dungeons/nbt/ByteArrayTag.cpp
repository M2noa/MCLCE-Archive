#include "Dungeons.h"

#include "ByteArrayTag.h"

ByteArrayTag::ByteArrayTag(const std::string& name) :
	Tag(name) {

}

ByteArrayTag::ByteArrayTag(const std::string& name, TagMemoryChunk data) :
	Tag(name)
	, data(data) {

}

Tag::Type ByteArrayTag::getId() const {
	return Type::ByteArray;
}

std::string ByteArrayTag::toString() const {
	std::stringstream ss;
	ss << "[" << data.size() << " bytes]";
	return ss.str();
}

bool ByteArrayTag::equals(const Tag& rhs) const {
	if (Tag::equals(rhs)) {
		ByteArrayTag& o = (ByteArrayTag&)rhs;
		return o.data == data;
	}
	return false;
}

Unique<Tag> ByteArrayTag::copy() const {
	return make_unique<ByteArrayTag>(getName(), data);
}

void ByteArrayTag::write(IDataOutput& dos) const /*throws IOException*/ {
	dos.writeInt(data.size());
	dos.writeBytes(data.data(), data.size());
}

void ByteArrayTag::load(IDataInput& dis) {	/*throws IOException*/
	auto ptr = data.alloc<byte>(dis.readInt());
	dis.readBytes(ptr, data.size());
}
