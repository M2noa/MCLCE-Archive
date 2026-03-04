#pragma once
using namespace std;

#include "Class.h"

class CompoundTag;

class SavedData :  public enable_shared_from_this<SavedData>
{
public:
	const wstring id;

private:
	bool dirty;

public:
	SavedData(const wstring& id);

	virtual void load(CompoundTag *tag) = 0;
	virtual void save(CompoundTag *tag) = 0;

	void setDirty();
	void setDirty(bool dirty);
	bool isDirty();
};