#pragma once
// 4J Added so that we can override the icon id used to calculate the texture UV's for each player

#include "Item.h"

class ClockItem : public Item
{
private:
	Icon **icons;
	static const wstring TEXTURE_PLAYER_ICON[XUSER_MAX_COUNT];

public:
	ClockItem(int id);

	virtual Icon *getIcon(int auxValue);

	//@Override
	void registerIcons(IconRegister *iconRegister);
};