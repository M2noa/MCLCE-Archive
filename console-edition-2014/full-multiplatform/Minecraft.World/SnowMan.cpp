#include "stdafx.h"
#include "net.minecraft.world.level.h"
#include "net.minecraft.world.entity.ai.goal.h"
#include "net.minecraft.world.entity.ai.goal.target.h"
#include "net.minecraft.world.entity.ai.navigation.h"
#include "net.minecraft.world.entity.monster.h"
#include "net.minecraft.world.entity.projectile.h"
#include "net.minecraft.world.phys.h"
#include "net.minecraft.world.level.tile.h"
#include "net.minecraft.world.damagesource.h"
#include "net.minecraft.world.level.biome.h"
#include "net.minecraft.world.item.h"
#include "SharedConstants.h"
#include "..\Minecraft.Client\Textures.h"
#include "SnowMan.h"
#include "SoundTypes.h"



SnowMan::SnowMan(Level *level) : Golem(level)
{
	// 4J Stu - This function call had to be moved here from the Entity ctor to ensure that
	// the derived version of the function is called
	this->defineSynchedData();

	// 4J Stu - This function call had to be moved here from the Entity ctor to ensure that the derived version of the function is called
	health = getMaxHealth();

	this->textureIdx = TN_MOB_SNOWMAN;// 4J was "/mob/snowman.png";
	this->setSize(0.4f, 1.8f);

	getNavigation()->setAvoidWater(true);
	goalSelector.addGoal(1, new ArrowAttackGoal(this, 0.25f, ArrowAttackGoal::SnowballType, SharedConstants::TICKS_PER_SECOND * 1));
	goalSelector.addGoal(2, new RandomStrollGoal(this, 0.2f));
	goalSelector.addGoal(3, new LookAtPlayerGoal(this, typeid(Player), 6));
	goalSelector.addGoal(4, new RandomLookAroundGoal(this));

	targetSelector.addGoal(1, new NearestAttackableTargetGoal(this, typeid(Monster), 16, 0, true));
}

bool SnowMan::useNewAi()
{
	return true;
}

int SnowMan::getMaxHealth()
{
	return 4;
}

void SnowMan::aiStep()
{
	Golem::aiStep();

	if (this->isInWaterOrRain()) hurt(DamageSource::drown, 1);

	{
		int xx = Mth::floor(x);
		int zz = Mth::floor(z);
		if (level->getBiome(xx, zz)->getTemperature() > 1)
		{
			hurt(DamageSource::onFire, 1);
		}
	}

	for (int i = 0; i < 4; i++)
	{
		int xx = Mth::floor(x + (i % 2 * 2 - 1) * 0.25f);
		int yy = Mth::floor(y);
		int zz = Mth::floor(z + ((i / 2) % 2 * 2 - 1) * 0.25f);
		if (level->getTile(xx, yy, zz) == 0)
		{
			if (level->getBiome(xx, zz)->getTemperature() < 0.8f)
			{
				if (Tile::topSnow->mayPlace(level, xx, yy, zz))
				{
					level->setTile(xx, yy, zz, Tile::topSnow_Id);
				}
			}
		}
	}
}

int SnowMan::getDeathLoot()
{
	return Item::snowBall_Id;
}


void SnowMan::dropDeathLoot(bool wasKilledByPlayer, int playerBonusLevel)
{
	// drop some feathers
	int count = random->nextInt(16);
	for (int i = 0; i < count; i++) {
		spawnAtLocation(Item::snowBall_Id, 1);
	}
}