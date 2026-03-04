#include "stdafx.h"
#include "MaterialColor.h"

MaterialColor **MaterialColor::colors;
		   
MaterialColor *MaterialColor::none = NULL;
MaterialColor *MaterialColor::grass = NULL;
MaterialColor *MaterialColor::sand = NULL;
MaterialColor *MaterialColor::cloth = NULL;
MaterialColor *MaterialColor::fire = NULL;
MaterialColor *MaterialColor::ice = NULL;
MaterialColor *MaterialColor::metal = NULL;
MaterialColor *MaterialColor::plant = NULL;
MaterialColor *MaterialColor::snow = NULL;
MaterialColor *MaterialColor::clay = NULL;
MaterialColor *MaterialColor::dirt = NULL;
MaterialColor *MaterialColor::stone = NULL;
MaterialColor *MaterialColor::water = NULL;
MaterialColor *MaterialColor::wood = NULL;

void MaterialColor::staticCtor()
{
	MaterialColor::colors = new MaterialColor *[16];

	MaterialColor::none = new MaterialColor(0, eMinecraftColour_Material_None);
	MaterialColor::grass = new MaterialColor(1, eMinecraftColour_Material_Grass);
	MaterialColor::sand = new MaterialColor(2, eMinecraftColour_Material_Sand);
	MaterialColor::cloth = new MaterialColor(3, eMinecraftColour_Material_Cloth);
	MaterialColor::fire = new MaterialColor(4, eMinecraftColour_Material_Fire);
	MaterialColor::ice = new MaterialColor(5, eMinecraftColour_Material_Ice);
	MaterialColor::metal = new MaterialColor(6, eMinecraftColour_Material_Metal);
	MaterialColor::plant = new MaterialColor(7, eMinecraftColour_Material_Plant);
	MaterialColor::snow = new MaterialColor(8, eMinecraftColour_Material_Snow);
	MaterialColor::clay = new MaterialColor(9, eMinecraftColour_Material_Clay);
	MaterialColor::dirt = new MaterialColor(10, eMinecraftColour_Material_Dirt);
	MaterialColor::stone = new MaterialColor(11, eMinecraftColour_Material_Stone);
	MaterialColor::water = new MaterialColor(12, eMinecraftColour_Material_Water);
	MaterialColor::wood = new MaterialColor(13, eMinecraftColour_Material_Wood);
}

MaterialColor::MaterialColor(int id, eMinecraftColour col)
{
	this->id = id;
	this->col = col;
	colors[id] = this;
}