#pragma once

#include "common_header.h"

#include "nbt/NamedTag.h"
#include "util/CollectionUtils.h"

#include "CommonTypes.h"
#include "TagMemoryChunk.h"

class ListTag;
typedef std::map<std::string, Unique<Tag> > TagMap;

class CompoundTag : public NamedTag {
public:

	CompoundTag();

	explicit CompoundTag(const std::string& name);

	CompoundTag(CompoundTag&& rhs);
	CompoundTag& operator=(CompoundTag&& rhs);

	CompoundTag(const CompoundTag&) = delete;
	CompoundTag operator=(const CompoundTag&) = delete;

	virtual ~CompoundTag();

	void write(IDataOutput& dos) const override /*throws IOException*/;

	void load(IDataInput& dis) override /*throws IOException*/;

	void getAllTags(std::vector<Tag*>& tags_) const;

	Type getId() const override;

	void put(const std::string& name, Unique<Tag> tag);
	void putByte(const std::string& name, char value);
	void putShort(const std::string& name, short value);
	void putInt(const std::string& name, int value);
	void putInt64(const std::string& name, int64_t value);
	void putFloat(const std::string& name, float value);
	void putDouble(const std::string& name, float value);
	void putString(const std::string& name, const std::string& value);
	void putByteArray(const std::string& name, TagMemoryChunk mem);
	void putCompound(const std::string& name, Unique<CompoundTag> value);
	void putBoolean(const std::string& string, bool val);

	Tag* get(const std::string& name) const;

	bool contains(const std::string& name) const;
	bool contains(const std::string& name, Type type) const;

	uint8_t getByte(const std::string& name) const;

	short getShort(const std::string& name) const;

	int getInt(const std::string& name) const;

	int64_t getInt64(const std::string& name) const;

	float getFloat(const std::string& name) const;

	double getDouble(const std::string& name) const;

	const std::string& getString(const std::string& name) const;

	TagMemoryChunk getByteArray(const std::string& name) const;

	TagMemoryChunk getIntArray(const std::string& name) const;

	const CompoundTag* getCompound(const std::string& name) const;

	const ListTag* getList(const std::string& name) const;

	bool getBoolean(const std::string& string) const;

	std::string toString() const override;

	void print(const std::string& prefix_, PrintStream& out) const override;

	bool isEmpty() const;

	Unique<Tag> copy() const override;

	bool equals(const Tag& obj) const override;

	Unique<CompoundTag> clone() const;

	void remove(const std::string& name);

	const TagMap& rawView() const;

	TagMap::const_iterator begin() const {
		return mTags.begin();
	}

	TagMap::const_iterator end() const {
		return mTags.end();
	}

private:

	TagMap mTags;
};
