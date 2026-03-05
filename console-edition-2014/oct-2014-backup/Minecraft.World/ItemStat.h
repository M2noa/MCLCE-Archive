#pragma once
using namespace std;

#include "Stat.h"

class ItemStat : public Stat
{
private:
	const int itemId;

public:
	ItemStat(int id, const wstring& name, int itemId);
    int getItemId();
};
