#include "Dungeons.h"

#include "nbt/Tag.h"

#include "nbt/EndTag.h"
#include "nbt/ByteTag.h"
#include "nbt/ShortTag.h"
#include "nbt/IntTag.h"
#include "nbt/Int64Tag.h"
#include "nbt/FloatTag.h"
#include "nbt/DoubleTag.h"
#include "nbt/ByteArrayTag.h"
#include "nbt/IntArrayTag.h"
#include "nbt/StringTag.h"
#include "nbt/ListTag.h"
#include "nbt/CompoundTag.h"

/*static*/ const std::string Tag::NullString = "";

Tag::Tag(const std::string& name){
}

/*virtual*/
bool Tag::equals(const Tag& rhs) const {
	return getId() == rhs.getId()
		   && getName() == rhs.getName();
}

/*virtual*/
void Tag::print(PrintStream& out) const {
	print("", out);
}

/*virtual*/
void Tag::print(const std::string& prefix, PrintStream& out) const {
	std::string name = getName();

	out.print(prefix);
	out.print(getTagName(getId()));
	if (name.length() > 0) {
		out.print("(\"" + name + "\")");
	}
	out.print(": ");
	out.println(toString());
}

/*virtual*/
Tag* Tag::setName(const std::string& name) {
	return this;
}

/*virtual*/
std::string Tag::getName() const {
	// DEBUG_TODO. This is used but crashes old clients (undefined behavior). What is the behavior if this is removed?
	//DEBUG_TODO; asserts when using "clone" function, disabled
	return "";
}

/*static*/
Unique<Tag> Tag::readNamedTag(IDataInput& dis, std::string& name) {
	auto type = (Type)dis.readByte();
	if (type == Tag::Type::End) {
		return make_unique<EndTag>();
	}

	name = dis.readString();
	auto tag = newTag(type, name);
	if (!tag) {
		return nullptr;
	}

	tag->load(dis);
	return tag;
}

/*static*/
void Tag::writeNamedTag(const std::string& name, const Tag& tag, IDataOutput& dos) {
	dos.writeByte(enum_cast(tag.getId()));
	if (tag.getId() == Tag::Type::End) {
		return;
	}

	dos.writeString(name);
	tag.write(dos);
}

/*static*/
Unique<Tag> Tag::newTag(Type type, const std::string& name){
	switch (type) {
	case Type::End:
		return make_unique<EndTag>();
	case Type::Byte:
		return make_unique<ByteTag>(name);
	case Type::Short:
		return make_unique<ShortTag>(name);
	case Type::Int:
		return make_unique<IntTag>(name);
	case Type::Int64:
		return make_unique<Int64Tag>(name);
	case Type::Float:
		return make_unique<FloatTag>(name);
	case Type::Double:
		return make_unique<DoubleTag>(name);
	case Type::ByteArray:
		return make_unique<ByteArrayTag>(name);
	case Type::IntArray:
		return make_unique<IntArrayTag>(name);
	case Type::String:
		return make_unique<StringTag>(name);
	case Type::List:
		return make_unique<ListTag>(name);
	case Type::Compound:
		return make_unique<CompoundTag>(name);
	}

	return nullptr;
}

/*static*/
std::string Tag::getTagName(Type type) {
	switch (type) {
	case Type::End:
		return "TAG_End";
	case Type::Byte:
		return "TAG_Byte";
	case Type::Short:
		return "TAG_Short";
	case Type::Int:
		return "TAG_Int";
	case Type::Int64:
		return "TAG_Long";
	case Type::Float:
		return "TAG_Float";
	case Type::Double:
		return "TAG_Double";
	case Type::ByteArray:
		return "TAG_Byte_Array";
	case Type::IntArray:
		return "TAG_Int_Array";
	case Type::String:
		return "TAG_String";
	case Type::List:
		return "TAG_List";
	case Type::Compound:
		return "TAG_Compound";
	default:
		return "UNKNOWN";
	}
}
