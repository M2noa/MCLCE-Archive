#pragma once

class TileEventData
{
private:
	int x, y, z;
	int tile;
	int paramA;
	int paramB;

public:
	TileEventData(int x, int y, int z, int tile, int paramA, int paramB);

	int getX();
	int getY();
	int getZ();
	int getParamA();
	int getParamB();
	int getTile();
	bool equals(TileEventData &ted);
};