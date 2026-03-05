#pragma once
#include "Material.h"

class DecorationMaterial : public Material
{
public:
	DecorationMaterial(MaterialColor *color) : Material(color) { makeDestroyedByHand(); }

	virtual bool isSolid() { return false; }
	virtual bool blocksLight() { return false; }
	virtual bool blocksMotion() { return false; }
};