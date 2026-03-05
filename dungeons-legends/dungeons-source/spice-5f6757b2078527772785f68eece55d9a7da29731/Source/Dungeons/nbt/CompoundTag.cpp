#include "Dungeons.h"

#include "CompoundTag.h"
#include "util/StringUtils.h"

#include "nbt/ByteTag.h"
#include "nbt/ShortTag.h"
#include "nbt/IntTag.h"
#include "nbt/Int64Tag.h"
#include "nbt/FloatTag.h"
#include "nbt/DoubleTag.h"
#include "nbt/ListTag.h"
#include "nbt/StringTag.h"
#include "nbt/ByteArrayTag.h"
#include "nbt/IntArrayTag.h"

CompoundTag::CompoundTag()
	: NamedTag(""){
}

CompoundTag::CompoundTag(const std::string& name)
	: NamedTag(name){
}

CompoundTag::CompoundTag(CompoundTag&& rhs)
	: NamedTag(rhs.getName())
	, mTags(std::move(rhs.mTags)){
}

CompoundTag& CompoundTag::operator=(CompoundTag&& rhs) {
	setName(rhs.getName());
	mTags = std::move(rhs.mTags);
	return *this;
}

CompoundTag::~CompoundTag(){
}

void CompoundTag::write(IDataOutput& dos) const {
	for (auto& pair : mTags) {
		Tag::writeNamedTag(pair.first, *pair.second, dos);
	}

	dos.writeByte(enum_cast(Tag::Type::End));
}

void CompoundTag::load(IDataInput& dis) {
	mTags.clear();
	Unique<Tag> tag;
	std::string _name;

	while ((tag = Tag::readNamedTag(dis, _name)) && tag->getId() != Tag::Type::End) {
		mTags[_name] = std::move(tag);
	}
}

void CompoundTag::getAllTags(std::vector<Tag*>& tags_) const {
	for (TagMap::const_iterator it = mTags.begin(); it != mTags.end(); ++it) {
		tags_.push_back(it->second.get());
	}
}

Tag::Type CompoundTag::getId() const {
	return Type::Compound;
}

void CompoundTag::put(const std::string& _name, Unique<Tag> tag) {
	tag->setName(_name);
	mTags[_name] = std::move(tag);
}

void CompoundTag::putByte(const std::string& _name, char value) {
	mTags[_name] = make_unique<ByteTag>(_name, value);
}

void CompoundTag::putShort(const std::string& _name, short value) {
	mTags[_name] = make_unique<ShortTag>(_name, value);
}

void CompoundTag::putInt(const std::string& _name, int value) {
	mTags[_name] = make_unique<IntTag>(_name, value);
}

void CompoundTag::putInt64(const std::string& _name, int64_t value) {
	mTags[_name] = make_unique<Int64Tag>(_name, value);
}

void CompoundTag::putFloat(const std::string& _name, float value) {
	mTags[_name] = make_unique<FloatTag>(_name, value);
}

void CompoundTag::putDouble(const std::string& _name, float value) {
	mTags[_name] = make_unique<DoubleTag>(_name, value);
}

void CompoundTag::putString(const std::string& _name, const std::string& value) {
	mTags[_name] = make_unique<StringTag>(_name, value);
}

void CompoundTag::putByteArray(const std::string& _name, TagMemoryChunk mem) {
	mTags[_name] = make_unique<ByteArrayTag>(_name, mem);
}

void CompoundTag::putCompound(const std::string& _name, Unique<CompoundTag> value) {
	value->setName(_name);
	mTags[_name] = std::move(value);
}

void CompoundTag::putBoolean(const std::string& string, bool val) {
	putByte(string, val ? (char)1 : 0);
}

Tag* CompoundTag::get(const std::string& _name) const {
	TagMap::const_iterator it = mTags.find(_name);
	if (it == mTags.end()) {
		return nullptr;
	}
	return it->second.get();
}

bool CompoundTag::contains(const std::string& _name) const {
	return mTags.find(_name) != mTags.end();
}

bool CompoundTag::contains(const std::string& _name, Type type) const {
	Tag* tag = get(_name);
	return tag && tag->getId() == type;
}

uint8_t CompoundTag::getByte(const std::string& _name) const {
	if (!contains(_name, Type::Byte)) {
		return (char)0;
	}
	return ((ByteTag*)get(_name))->data;
}

short CompoundTag::getShort(const std::string& _name) const {
	if (!contains(_name, Type::Short)) {
		return (short)0;
	}

	return ((ShortTag*)get(_name))->data;
}

int CompoundTag::getInt(const std::string& _name) const {
	if (!contains(_name, Type::Int)) {
		return 0;
	}

	return ((IntTag*)get(_name))->data;
}

int64_t CompoundTag::getInt64(const std::string& _name) const {
	if (!contains(_name, Type::Int64)) {
		return (int64_t)0;
	}
	return ((Int64Tag*)get(_name))->data;
}

float CompoundTag::getFloat(const std::string& _name) const {
	if (!contains(_name, Type::Float)) {
		return 0.0f;
	}

	return ((FloatTag*)get(_name))->data;
}

double CompoundTag::getDouble(const std::string& _name) const {
	if (!contains(_name, Type::Double)) {
		return 0.0;
	}
	return ((DoubleTag*)get(_name))->data;
}

const std::string& CompoundTag::getString(const std::string& _name) const {
	if (!contains(_name, Type::String)) {
		return Util::EMPTY_STRING;
	}

	return ((StringTag*)get(_name))->data;
}

TagMemoryChunk CompoundTag::getByteArray(const std::string& _name) const {
	if (!contains(_name, Type::ByteArray)) {
		return TagMemoryChunk();
	}

	return ((ByteArrayTag*)get(_name))->data;
}

TagMemoryChunk CompoundTag::getIntArray(const std::string& _name) const {
	if (!contains(_name, Type::IntArray)) {
		return TagMemoryChunk();
	}

	return ((IntArrayTag*)get(_name))->data;
}

const CompoundTag* CompoundTag::getCompound(const std::string& _name) const {
	if (!contains(_name, Type::Compound)) {
		return nullptr;
	}

	auto tag = (CompoundTag*)get(_name);
	DEBUG_ASSERT(tag, "Not found anyway?");
	return tag;
}

const ListTag* CompoundTag::getList(const std::string& _name) const {
	if (contains(_name, Type::List)) {
		return (ListTag*)get(_name);
	}

	return nullptr;
}

bool CompoundTag::getBoolean(const std::string& string) const {
	return getByte(string) != 0;
}

std::string CompoundTag::toString() const {
	std::stringstream ss;
	ss << mTags.size() << " entries";
	return ss.str();
}

void CompoundTag::print(const std::string& prefix_, PrintStream& out) const {
	NamedTag::print(prefix_, out);
	std::string prefix = prefix_;
	out.print(prefix);
	out.println("{");
	prefix += "   ";

	for (TagMap::const_iterator it = mTags.begin(); it != mTags.end(); ++it) {
		(it->second)->print(prefix, out);
	}

	out.print(prefix_);
	out.println("}");
}

bool CompoundTag::isEmpty() const {
	return mTags.empty();
}

Unique<Tag> CompoundTag::copy() const {
	return clone();
}

bool CompoundTag::equals(const Tag& obj) const {
	if (NamedTag::equals(obj)) {
		CompoundTag& o = (CompoundTag&)obj;

		if (mTags.size() != o.mTags.size()) {
			return false;
		}

		for (TagMap::const_iterator it = mTags.begin(), jt = o.mTags.begin(); it != mTags.end(); ++it, ++jt) {
			if (it->first != jt->first) {
				return false;
			}

			if (!it->second->equals(*jt->second)) {
				return false;
			}
		}

		return true;
	}
	return false;
}

Unique<CompoundTag> CompoundTag::clone() const {
	Unique<CompoundTag> compoundTag = std::make_unique<CompoundTag>(getName());

	// deep copy
	for (auto&& pair : mTags) {
		compoundTag->put(pair.first, pair.second->copy());
	}

	return compoundTag;
}

const TagMap& CompoundTag::rawView() const {
	return mTags;
}

void CompoundTag::remove(const std::string& _name) {
	mTags.erase(_name);
}
