#pragma once

#include "nbt/CompoundTag.h"
#include "util/DataIO.h"

class NbtIo {
public:

	static Unique<CompoundTag> read(IDataInput& dis) {
		std::string name;
		auto tag = Tag::readNamedTag(dis, name);
		if (tag && tag->getId() == Tag::Type::Compound) {
			return Unique<CompoundTag>((CompoundTag*)tag.release());
		}
		else{
			return nullptr;
		}
	}

	static void write(const CompoundTag* tag, IDataOutput& dos) {
		if (!tag) {
			return;
		}
		Tag::writeNamedTag(tag->getName(), *tag, dos);
	}

};
