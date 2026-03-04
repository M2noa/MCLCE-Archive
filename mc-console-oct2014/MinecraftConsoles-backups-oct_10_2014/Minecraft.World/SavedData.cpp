#include "stdafx.h"
#include "SavedData.h"



SavedData::SavedData(const wstring& id) : id( id )
{
	dirty = false;
}

void SavedData::setDirty()
{
	setDirty(true);
}

void SavedData::setDirty(bool dirty)
{
	this->dirty = dirty;
}

bool SavedData::isDirty()
{
	return dirty;
}