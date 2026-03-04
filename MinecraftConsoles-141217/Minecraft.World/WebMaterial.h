#include "Material.h"

// 4J added, Java version just does a local alteration when instantiating the Material for webs to get the same thing
class WebMaterial : public Material
{
public:
	WebMaterial(MaterialColor *color) : Material(color) {}
	virtual bool blocksMotion() { return false; }
};