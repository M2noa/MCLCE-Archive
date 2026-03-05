#include "stdafx.h"
#include "net.minecraft.world.level.h"
#include "Giant.h"
#include "..\Minecraft.Client\Textures.h"



Giant::Giant(Level *level) : Monster( level )
{
	// 4J Stu - This function call had to be moved here from the Entity ctor to ensure that
	// the derived version of the function is called
	this->defineSynchedData();

	// 4J Stu - This function call had to be moved here from the Entity ctor to ensure that the derived version of the function is called
	health = getMaxHealth();

	this->textureIdx = TN_MOB_ZOMBIE;	// 4J was L"/mob/zombie.png";
	runSpeed = 0.5f;
	attackDamage = 50;
	this->heightOffset*=6;
	this->setSize(bbWidth * 6, bbHeight * 6);
}

int Giant::getMaxHealth()
{
	return 100;
}

float Giant::getWalkTargetValue(int x, int y, int z)
{
	return level->getBrightness(x, y, z)-0.5f;
}