#pragma once

#include "common_header.h"

#include "nbt/Tag.h"

class CompoundTag;

//template <class T>
class ListTag : public Tag {

	typedef std::vector<Unique<Tag> > List;

public:

	ListTag();

	ListTag(const std::string& name);

	void write(IDataOutput& dos) const override /*throws IOException*/;

	//@SuppressWarnings("unchecked")
	void load(IDataInput& dis) override /*throws IOException*/;

	Type getId() const override;

	std::string toString() const override;

	void print(const std::string& prefix_, PrintStream& out) const override;

	void add(Unique<Tag> tag);

	Tag* get(int index) const;
	float getFloat(int index) const;
	int getInt(int index) const;
	double getDouble(int index) const;

	int size() const;

	//@Override
	Unique<Tag> copy() const override;

	//@SuppressWarnings("rawtypes")
	//@Override
	bool equals(const Tag& rhs) const override;

	const CompoundTag* getCompound(size_t index) const;

	void deleteChildren() override;

private:

	List mList;
	Type mType;
};

class ListTagFloatAdder {
public:

	ListTagFloatAdder(){
	}

	ListTagFloatAdder(float f){
		operator() (f);
	}

	ListTagFloatAdder(Unique<ListTag> tag)
		: mTag(std::move(tag)){
	}

	ListTagFloatAdder& operator() (const std::string& name, float f);
	ListTagFloatAdder& operator() (float f);

	Unique<ListTag> done() {
		return std::move(mTag);
	}

	Unique<ListTag> mTag;
};

class ListTagIntAdder {
public:

	ListTagIntAdder(){
	}

	ListTagIntAdder(int i){
		operator() (i);
	}

	ListTagIntAdder(Unique<ListTag> tag)
		: mTag(std::move(tag)){
	}

	ListTagIntAdder& operator() (const std::string& name, int i);
	ListTagIntAdder& operator() (int i);

	Unique<ListTag> done() {
		return std::move(mTag);
	}

	Unique<ListTag> mTag;
};
