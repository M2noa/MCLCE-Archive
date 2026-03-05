#include "stdafx.h"

#include "TileEventData.h"

TileEventData::TileEventData(int x, int y, int z, int tile, int paramA, int paramB)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->paramA = paramA;
	this->paramB = paramB;
	this->tile = tile;
}

int TileEventData::getX()
{
	return x;
}

int TileEventData::getY()
{
	return y;
}

int TileEventData::getZ()
{
	return z;
}

int TileEventData::getParamA()
{
	return paramA;
}

int TileEventData::getParamB()
{
	return paramB;
}

int TileEventData::getTile()
{
	return tile;
}

bool TileEventData::equals(TileEventData &ted)
{
	return x == ted.x && y == ted.y && z == ted.z && paramA == ted.paramA && paramB == ted.paramB && tile == ted.tile;
}