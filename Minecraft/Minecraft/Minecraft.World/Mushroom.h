#pragma once
#include "Bush.h"

class Random;

class Mushroom : public Bush
{
	friend class Tile;
private:
	wstring texture;
protected:
	Mushroom(int id, const wstring &texture);
public:
    virtual void updateDefaultShape(); // 4J Added override
	virtual void tick(Level *level, int x, int y, int z, Random *random);
	virtual bool mayPlace(Level *level, int x, int y, int z);
protected:
	virtual bool mayPlaceOn(int tile);
public:
	virtual bool canSurvive(Level *level, int x, int y, int z);
	bool growTree(Level *level, int x, int y, int z, Random *random);
	void registerIcons(IconRegister *iconRegister);
};
