#pragma once

#include "DirectionalTile_SPU.h"

class CocoaTile_SPU : public DirectionalTile_SPU
{
public:	
	static const int COCOA_TEXTURES_LENGTH = 3;

public:
	CocoaTile_SPU(int id) : DirectionalTile_SPU(id) {}

// 	virtual Icon *getTexture(int face, int data);
// 	virtual Icon *getTextureForAge(int age);
	virtual int getRenderShape() { return SHAPE_COCOA; }
	virtual bool isCubeShaped() { return false; }
	virtual bool isSolidRender(bool isServerLevel = false) { return false; }
};