#pragma once

#include "Tile_SPU.h"

class DirectionalTile_SPU : public Tile_SPU
{
public:
	static const int DIRECTION_MASK = 0x3;
	static const int DIRECTION_INV_MASK = 0xC;

protected:
	DirectionalTile_SPU(int id) : Tile_SPU(id) {}

public:
	static int getDirection(int data) { 	return data & DIRECTION_MASK; }
};