#pragma once

/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#include <string>

class CompoundTag;

class SavedData {
public:

	SavedData(std::string const& id) :
		mId(id) {
	}

	virtual ~SavedData() {
	}

	virtual void load(const CompoundTag& tag) = 0;
	virtual void save(CompoundTag& tag) = 0;

	void setDirty(bool dirt = true) {
		mDirty = dirt;
	}

	bool isDirty() const {
		return mDirty;
	}

	std::string getId() const { return mId; }

private:

	bool mDirty;
	std::string mId;
};
