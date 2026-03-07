#pragma once
#include "Material.h"

class GasMaterial : public Material
{
public:
	GasMaterial(MaterialColor *color) : Material(color) { replaceable(); }

	virtual bool isSolid() { return false; }
	virtual bool blocksLight() { return false; }
	virtual bool blocksMotion() { return false; }
};