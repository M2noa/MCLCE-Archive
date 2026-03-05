#pragma once
#include "Material.h"

class PortalMaterial : public Material
{
public:
	PortalMaterial(MaterialColor *color) : Material(color) { }

	virtual bool isSolid() { return false; }
	virtual bool blocksLight() { return false; }
	virtual bool blocksMotion() { return false; }
};