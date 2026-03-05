#pragma once

#include "Bush_SPU.h"

class Random;

class Sapling_SPU : public Bush_SPU
{	
	static const int TYPE_MASK = 3;

public:
	Sapling_SPU(int id) : Bush_SPU(id) {}
	Icon_SPU *getTexture(int face, int data) { 	data = data & TYPE_MASK;	return &ms_pTileData->sapling_icons[data]; }
};