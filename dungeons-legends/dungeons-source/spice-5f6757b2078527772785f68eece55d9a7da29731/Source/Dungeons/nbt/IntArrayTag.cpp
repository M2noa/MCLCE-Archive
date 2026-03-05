#include "Dungeons.h"

#include "IntArrayTag.h"

IntArrayTag::IntArrayTag(const std::string& name) :
	Tag(name){

}

IntArrayTag::IntArrayTag(const std::string& name, TagMemoryChunk data) :
	Tag(name)
	, data(data){

}

Tag::Type IntArrayTag::getId() const {
	return Type::IntArray;
}

std::string IntArrayTag::toString() const {
	std::stringstream ss;
	ss << "[" << data.size() << " ints]";
	return ss.str();
}

bool IntArrayTag::equals(const Tag& rhs) const {
	if (Tag::equals(rhs)) {
		IntArrayTag& o = (IntArrayTag&)rhs;
		return o.data == data;
	}
	return false;
}

Unique<Tag> IntArrayTag::copy() const {
	return make_unique<IntArrayTag>(getName(), data);
}

void IntArrayTag::write(IDataOutput& dos) const	/*throws IOException*/
{
	dos.writeInt(data.size());

	for (size_t i = 0; i < data.size(); i++) {
		dos.writeInt(((int*)data.data())[i]);
	}
}

void IntArrayTag::load(IDataInput& dis){/*throws IOException*/
	auto ptr = data.alloc<int>(dis.readInt());

	for (size_t i = 0; i < data.size(); i++) {
		ptr[i] = dis.readInt();
	}
}
