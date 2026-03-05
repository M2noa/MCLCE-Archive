#include "Dungeons.h"

#include "ListTag.h"
#include "FloatTag.h"
#include "IntTag.h"
#include "DoubleTag.h"
#include "CompoundTag.h"

ListTag::ListTag() :
	Tag(""){

}

ListTag::ListTag(const std::string& name) :
	Tag(name){

}

void ListTag::write(IDataOutput& dos) const	/*throws IOException*/
{
	dos.writeByte(enum_cast(mList.empty() ? Type::Byte : mList.front()->getId()));
	dos.writeInt((int)mList.size());

	for (auto& a : mList) {
		a->write(dos);
	}
}

void ListTag::load(IDataInput& dis){/*throws IOException*/
	mType = (Type)dis.readByte();
	int size = dis.readInt();

	mList.clear();// = List();

	for (int i = 0; i < size; i++) {
		auto tag = Tag::newTag(mType, NullString);
		tag->load(dis);
		mList.emplace_back(std::move(tag));
	}
}

Tag::Type ListTag::getId() const {
	return Type::List;
}

std::string ListTag::toString() const {
	std::stringstream ss;
	ss << mList.size() << " entries of type " << Tag::getTagName(mType);
	return ss.str();
}

void ListTag::print(const std::string& prefix_, PrintStream& out) const {
	Tag::print(prefix_, out);

	std::string prefix = prefix_;
	out.print(prefix);
	out.println("{");
	prefix += "   ";

	for (List::const_iterator it = mList.begin(); it != mList.end(); ++it) {
		(*it)->print(prefix, out);
	}
	out.print(prefix_);
	out.println("}");
}

void ListTag::add(Unique<Tag> tag){
	mType = tag->getId();
	mList.emplace_back(std::move(tag));
}

Tag* ListTag::get(int index) const {
	if (index >= size()) {
		return nullptr;
	}
	return mList[index].get();
}

float ListTag::getFloat(int index) const {
	auto tag = get(index);
	if (!tag) {
		return 0;
	}
	if (tag->getId() != Type::Float) {
		return 0;
	}
	return ((FloatTag*)tag)->data;
}

int ListTag::getInt(int index) const {
	auto tag = get(index);
	if (!tag) {
		return 0;
	}
	if (tag->getId() != Type::Int) {
		return 0;
	}
	return ((IntTag*)tag)->data;
}

double ListTag::getDouble(int index) const {
	auto tag = get(index);
	if (!tag) {
		return 0;
	}
	if (tag->getId() != Type::Double) {
		return 0;
	}
	return ((DoubleTag*)tag)->data;
}

int ListTag::size() const {
	return (int)mList.size();
}

Unique<Tag> ListTag::copy() const {
	auto res = make_unique<ListTag>(getName());
	res->mType = mType;

	for (List::const_iterator it = mList.begin(); it != mList.end(); ++it) {
		res->mList.emplace_back((*it)->copy());
	}
	return std::move(res);
}

bool ListTag::equals(const Tag& rhs) const {
	if (Tag::equals(rhs)) {
		ListTag& o = (ListTag&)rhs;
		if (mType == o.mType && mList.size() == o.mList.size()) {
			for (List::const_iterator it = mList.begin(), jt = o.mList.begin(); it != mList.end(); ++it, ++jt) {
				if (!(*it)->equals(**jt)) {
					return false;
				}
			}
			return true;
		}
	}
	return false;
}

const CompoundTag* ListTag::getCompound(size_t index) const {
	// if the index is within the list bounds
	if (index < mList.size()) {
		const Unique<Tag>& tag = mList[index];
		if (tag->getId() == Tag::Type::Compound) {
			return (const CompoundTag*) tag.get();
		}
	}

	// if the index isn't in the list bounds,
	// or the tag at that index isn't a compound tag
	// return null
	return nullptr;

}

void ListTag::deleteChildren(){
	mList.clear();
}

ListTagFloatAdder& ListTagFloatAdder::operator()(const std::string& name, float f){
	if (!mTag) {
		mTag = make_unique<ListTag>();
	}
	mTag->add(make_unique<FloatTag>(name, f));
	return *this;
}

ListTagFloatAdder& ListTagFloatAdder::operator()(float f){
	return operator() ("", f);
}

ListTagIntAdder& ListTagIntAdder::operator()(const std::string& name, int i){
	if (!mTag) {
		mTag = make_unique<ListTag>();
	}
	mTag->add(make_unique<IntTag>(name, i));
	return *this;
}

ListTagIntAdder& ListTagIntAdder::operator()(int i){
	return operator() ("", i);
}
