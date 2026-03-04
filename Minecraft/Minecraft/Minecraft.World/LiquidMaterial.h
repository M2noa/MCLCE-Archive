#pragma once
#include "Material.h"

class LiquidMaterial : public Material
{
public:
	LiquidMaterial(MaterialColor *color) : Material(color) { replaceable(); destroyOnPush(); }

	virtual bool isLiquid() { return true; }
	virtual bool blocksMotion() { return false; }
	virtual bool isSolid() { return false; }
};