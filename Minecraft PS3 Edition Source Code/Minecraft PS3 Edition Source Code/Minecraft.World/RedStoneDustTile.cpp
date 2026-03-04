#include "stdafx.h"
#include "..\Minecraft.Client\Minecraft.h"
#include "RedStoneDustTile.h"
#include "net.minecraft.world.item.h"
#include "net.minecraft.world.level.h"
#include "net.minecraft.world.h"
#include "Direction.h"
#include "DiodeTile.h"

// AP - added for Vita to set Alpha Cut out
#include "IntBuffer.h"
#include "..\Minecraft.Client\Tesselator.h"

const wstring RedStoneDustTile::TEXTURE_CROSS = L"redstoneDust_cross";
const wstring RedStoneDustTile::TEXTURE_LINE = L"redstoneDust_line";
const wstring RedStoneDustTile::TEXTURE_CROSS_OVERLAY = L"redstoneDust_cross_overlay";
const wstring RedStoneDustTile::TEXTURE_LINE_OVERLAY = L"redstoneDust_line_overlay";

RedStoneDustTile::RedStoneDustTile(int id) : Tile(id, Material::decoration,isSolidRender())
{
	shouldSignal = true;

	updateDefaultShape();

	iconCross = NULL;
	iconLine = NULL;
	iconCrossOver = NULL;
	iconLineOver = NULL;
}

// 4J Added override
void RedStoneDustTile::updateDefaultShape()
{
	setShape(0, 0, 0, 1, 1 / 16.0f, 1);
}

AABB *RedStoneDustTile::getAABB(Level *level, int x, int y, int z)
{
	return NULL;
}

bool RedStoneDustTile::isSolidRender(bool isServerLevel)
{
	return false;
}

bool RedStoneDustTile::isCubeShaped()
{
	return false;
}

int RedStoneDustTile::getRenderShape()
{
	return Tile::SHAPE_RED_DUST;
}

int RedStoneDustTile::getColor() const
{
	return Minecraft::GetInstance()->getColourTable()->getColor(eMinecraftColour_Tile_RedstoneDust); // 0x800000;
}

int RedStoneDustTile::getColor(LevelSource *level, int x, int y, int z)
{
	return Minecraft::GetInstance()->getColourTable()->getColor(eMinecraftColour_Tile_RedstoneDust); // 0x800000;
}

int RedStoneDustTile::getColor(LevelSource *level, int x, int y, int z, int data)
{
	return Minecraft::GetInstance()->getColourTable()->getColor(eMinecraftColour_Tile_RedstoneDust); // 0x800000;
}

bool RedStoneDustTile::mayPlace(Level *level, int x, int y, int z)
{
	return level->isTopSolidBlocking(x, y - 1, z) || level->getTile(x, y - 1, z) == Tile::lightGem_Id;
}

void RedStoneDustTile::updatePowerStrength(Level *level, int x, int y, int z)
{
	updatePowerStrength(level, x, y, z, x, y, z);
	
	vector<TilePos> updates = vector<TilePos>(toUpdate.begin(), toUpdate.end());
	toUpdate.clear();

	AUTO_VAR(itEnd, updates.end());
	for(AUTO_VAR(it, updates.begin()); it != itEnd; it++)
	{
		TilePos tp = *it;
		level->updateNeighborsAt(tp.x, tp.y, tp.z, id);
	}
}

void RedStoneDustTile::updatePowerStrength(Level *level, int x, int y, int z, int xFrom, int yFrom, int zFrom)
{
	int old = level->getData(x, y, z);
	int target = 0;

	this->shouldSignal = false;
	bool neighborSignal = level->hasNeighborSignal(x, y, z);
	this->shouldSignal = true;

	if (neighborSignal)
	{
		target = 15;
	}
	else
	{
		for (int i = 0; i < 4; i++)
		{
			int xt = x;
			int zt = z;
			if (i == 0) xt--;
			if (i == 1) xt++;
			if (i == 2) zt--;
			if (i == 3) zt++;

			if (xt != xFrom || y != yFrom || zt != zFrom) target = checkTarget(level, xt, y, zt, target);
			if (level->isSolidBlockingTile(xt, y, zt) && !level->isSolidBlockingTile(x, y + 1, z))
			{
				if (xt != xFrom || y + 1 != yFrom || zt != zFrom) target = checkTarget(level, xt, y + 1, zt, target);
			}
			else if (!level->isSolidBlockingTile(xt, y, zt))
			{
				if (xt != xFrom || y - 1 != yFrom || zt != zFrom) target = checkTarget(level, xt, y - 1, zt, target);
			}
		}
		if (target > 0) target--;
		else target = 0;
	}

	if (old != target)
	{
		level->noNeighborUpdate = true;
		level->setData(x, y, z, target);
		level->setTilesDirty(x, y, z, x, y, z);
		level->noNeighborUpdate = false;

		for (int i = 0; i < 4; i++)
		{
			int xt = x;
			int zt = z;
			int yt = y - 1;
			if (i == 0) xt--;
			if (i == 1) xt++;
			if (i == 2) zt--;
			if (i == 3) zt++;

			if (level->isSolidBlockingTile(xt, y, zt)) yt += 2;

			int current = 0;
			current = checkTarget(level, xt, y, zt, -1);
			target = level->getData(x, y, z);
			if (target > 0) target--;
			if (current >= 0 && current != target)
			{
				updatePowerStrength(level, xt, y, zt, x, y, z);
			}
			current = checkTarget(level, xt, yt, zt, -1);
			target = level->getData(x, y, z);
			if (target > 0) target--;
			if (current >= 0 && current != target)
			{
				updatePowerStrength(level, xt, yt, zt, x, y, z);
			}
		}

		if (old < target || target == 0)
		{
			toUpdate.insert(TilePos(x, y, z));
			toUpdate.insert(TilePos(x - 1, y, z));
			toUpdate.insert(TilePos(x + 1, y, z));
			toUpdate.insert(TilePos(x, y - 1, z));
			toUpdate.insert(TilePos(x, y + 1, z));
			toUpdate.insert(TilePos(x, y, z - 1));
			toUpdate.insert(TilePos(x, y, z + 1));
		}
	}
}

void RedStoneDustTile::checkCornerChangeAt(Level *level, int x, int y, int z)
{
	if (level->getTile(x, y, z) != id) return;

	level->updateNeighborsAt(x, y, z, id);
	level->updateNeighborsAt(x - 1, y, z, id);
	level->updateNeighborsAt(x + 1, y, z, id);
	level->updateNeighborsAt(x, y, z - 1, id);
	level->updateNeighborsAt(x, y, z + 1, id);
		 
	level->updateNeighborsAt(x, y - 1, z, id);
	level->updateNeighborsAt(x, y + 1, z, id);
}

void RedStoneDustTile::onPlace(Level *level, int x, int y, int z)
{
	Tile::onPlace(level, x, y, z);
	if (level->isClientSide) return;

	updatePowerStrength(level, x, y, z);
	level->updateNeighborsAt(x, y + 1, z, id);
	level->updateNeighborsAt(x, y - 1, z, id);

	checkCornerChangeAt(level, x - 1, y, z);
	checkCornerChangeAt(level, x + 1, y, z);
	checkCornerChangeAt(level, x, y, z - 1);
	checkCornerChangeAt(level, x, y, z + 1);

	if (level->isSolidBlockingTile(x - 1, y, z)) checkCornerChangeAt(level, x - 1, y + 1, z);
	else checkCornerChangeAt(level, x - 1, y - 1, z);
	if (level->isSolidBlockingTile(x + 1, y, z)) checkCornerChangeAt(level, x + 1, y + 1, z);
	else checkCornerChangeAt(level, x + 1, y - 1, z);
	if (level->isSolidBlockingTile(x, y, z - 1)) checkCornerChangeAt(level, x, y + 1, z - 1);
	else checkCornerChangeAt(level, x, y - 1, z - 1);
	if (level->isSolidBlockingTile(x, y, z + 1)) checkCornerChangeAt(level, x, y + 1, z + 1);
	else checkCornerChangeAt(level, x, y - 1, z + 1);

}

void RedStoneDustTile::onRemove(Level *level, int x, int y, int z, int id, int data)
{
	Tile::onRemove(level, x, y, z, id, data);
	if (level->isClientSide) return;

	level->updateNeighborsAt(x, y + 1, z, this->id);
	level->updateNeighborsAt(x, y - 1, z, this->id);
	level->updateNeighborsAt(x + 1, y, z, this->id);
	level->updateNeighborsAt(x - 1, y, z, this->id);
	level->updateNeighborsAt(x, y, z + 1, this->id);
	level->updateNeighborsAt(x, y, z - 1, this->id);
	updatePowerStrength(level, x, y, z);

	checkCornerChangeAt(level, x - 1, y, z);
	checkCornerChangeAt(level, x + 1, y, z);
	checkCornerChangeAt(level, x, y, z - 1);
	checkCornerChangeAt(level, x, y, z + 1);

	if (level->isSolidBlockingTile(x - 1, y, z)) checkCornerChangeAt(level, x - 1, y + 1, z);
	else checkCornerChangeAt(level, x - 1, y - 1, z);
	if (level->isSolidBlockingTile(x + 1, y, z)) checkCornerChangeAt(level, x + 1, y + 1, z);
	else checkCornerChangeAt(level, x + 1, y - 1, z);
	if (level->isSolidBlockingTile(x, y, z - 1)) checkCornerChangeAt(level, x, y + 1, z - 1);
	else checkCornerChangeAt(level, x, y - 1, z - 1);
	if (level->isSolidBlockingTile(x, y, z + 1)) checkCornerChangeAt(level, x, y + 1, z + 1);
	else checkCornerChangeAt(level, x, y - 1, z + 1);
}

int RedStoneDustTile::checkTarget(Level *level, int x, int y, int z, int target)
{
	if (level->getTile(x, y, z) != id) return target;
	int d = level->getData(x, y, z);
	if (d > target) return d;
	return target;
}

void RedStoneDustTile::neighborChanged(Level *level, int x, int y, int z, int type)
{
	if (level->isClientSide) return;
	int face = level->getData(x, y, z);

	bool ok = mayPlace(level, x, y, z);

	if (ok)
	{
		updatePowerStrength(level, x, y, z);
	}
	else
	{
		spawnResources(level, x, y, z, face, 0);
		level->setTile(x, y, z, 0);
	}

	Tile::neighborChanged(level, x, y, z, type);
}

int RedStoneDustTile::getResource(int data, Random *random, int playerBonusLevel)
{
	return Item::redStone->id;
}

bool RedStoneDustTile::getDirectSignal(Level *level, int x, int y, int z, int dir)
{
	if (!shouldSignal) return false;
	return getSignal(level, x, y, z, dir);
}

bool RedStoneDustTile::getSignal(LevelSource *level, int x, int y, int z, int dir)
{
	if (!shouldSignal) return false;
	if (level->getData(x, y, z) == 0) return false;

	if (dir == 1) return true;

    bool w = RedStoneDustTile::shouldReceivePowerFrom(level, x - 1, y, z, Direction::WEST)
            || (!level->isSolidBlockingTile(x - 1, y, z) && RedStoneDustTile::shouldReceivePowerFrom(level, x - 1, y - 1, z, Direction::UNDEFINED));
    bool e = RedStoneDustTile::shouldReceivePowerFrom(level, x + 1, y, z, Direction::EAST)
            || (!level->isSolidBlockingTile(x + 1, y, z) && RedStoneDustTile::shouldReceivePowerFrom(level, x + 1, y - 1, z, Direction::UNDEFINED));
    bool n = RedStoneDustTile::shouldReceivePowerFrom(level, x, y, z - 1, Direction::NORTH)
            || (!level->isSolidBlockingTile(x, y, z - 1) && RedStoneDustTile::shouldReceivePowerFrom(level, x, y - 1, z - 1, Direction::UNDEFINED));
    bool s = RedStoneDustTile::shouldReceivePowerFrom(level, x, y, z + 1, Direction::SOUTH)
            || (!level->isSolidBlockingTile(x, y, z + 1) && RedStoneDustTile::shouldReceivePowerFrom(level, x, y - 1, z + 1, Direction::UNDEFINED));

	if (!level->isSolidBlockingTile(x, y + 1, z))
	{
        if (level->isSolidBlockingTile(x - 1, y, z) && RedStoneDustTile::shouldReceivePowerFrom(level, x - 1, y + 1, z, Direction::UNDEFINED)) w = true;
        if (level->isSolidBlockingTile(x + 1, y, z) && RedStoneDustTile::shouldReceivePowerFrom(level, x + 1, y + 1, z, Direction::UNDEFINED)) e = true;
        if (level->isSolidBlockingTile(x, y, z - 1) && RedStoneDustTile::shouldReceivePowerFrom(level, x, y + 1, z - 1, Direction::UNDEFINED)) n = true;
        if (level->isSolidBlockingTile(x, y, z + 1) && RedStoneDustTile::shouldReceivePowerFrom(level, x, y + 1, z + 1, Direction::UNDEFINED)) s = true;
	}

	if (!n && !e && !w && !s && (dir >= 2 && dir <= 5)) return true;

	if (dir == 2 && n && (!w && !e)) return true;
	if (dir == 3 && s && (!w && !e)) return true;
	if (dir == 4 && w && (!n && !s)) return true;
	if (dir == 5 && e && (!n && !s)) return true;

	return false;

}


bool RedStoneDustTile::isSignalSource()
{
	return shouldSignal;
}

void RedStoneDustTile::animateTick(Level *level, int x, int y, int z, Random *random)
{
	int data = level->getData(x, y, z);
	if (data > 0)
	{
		double xx = x + 0.5 + (random->nextFloat() - 0.5) * 0.2;
		double yy = y + 1 / 16.0f;
		double zz = z + 0.5 + (random->nextFloat() - 0.5) * 0.2;
		// use the x movement variable to determine particle color

		// 4J Stu - Unused
		//float pow = (data / 15.0f);
		//float red = pow * 0.6f + 0.4f;
		//if (data == 0) red = 0;

		//float green = pow * pow * 0.7f - 0.5f;
		//float blue = pow * pow * 0.6f - 0.7f;
		//if (green < 0) green = 0;
		//if (blue < 0) blue = 0;

		unsigned int colour = 0;
		if(data == 0)
		{
			colour = Minecraft::GetInstance()->getColourTable()->getColor( eMinecraftColour_Tile_RedstoneDustUnlit );
		}
		else
		{
			unsigned int minColour = Minecraft::GetInstance()->getColourTable()->getColor( eMinecraftColour_Tile_RedstoneDustLitMin );
			unsigned int maxColour = Minecraft::GetInstance()->getColourTable()->getColor( eMinecraftColour_Tile_RedstoneDustLitMax );

			byte redComponent = ((minColour>>16)&0xFF) + (( (maxColour>>16)&0xFF - (minColour>>16)&0xFF)*( (data-1)/14.0f));
			byte greenComponent = ((minColour>>8)&0xFF) + (( (maxColour>>8)&0xFF - (minColour>>8)&0xFF)*( (data-1)/14.0f));
			byte blueComponent = ((minColour)&0xFF) + (( (maxColour)&0xFF - (minColour)&0xFF)*( (data-1)/14.0f));

			colour = redComponent<<16 | greenComponent<<8 | blueComponent;
		}

		float red = ((colour>>16)&0xFF)/255.0f;
		float green = ((colour>>8)&0xFF)/255.0f;
		float blue = (colour&0xFF)/255.0f;

		level->addParticle(eParticleType_reddust, xx, yy, zz, red, green, blue);
	}
}


bool RedStoneDustTile::shouldConnectTo(LevelSource *level, int x, int y, int z, int direction)
{
	int t = level->getTile(x, y, z);
	if (t == Tile::redStoneDust_Id) return true;
	if (t == 0) return false;
    if (t == Tile::diode_off_Id || t == Tile::diode_on_Id)
	{
        int data = level->getData(x, y, z);
        return direction == (data & DiodeTile::DIRECTION_MASK) || direction == Direction::DIRECTION_OPPOSITE[data & DiodeTile::DIRECTION_MASK];
    }
	else if (Tile::tiles[t]->isSignalSource() && direction != Direction::UNDEFINED) return true;

	return false;
}

bool RedStoneDustTile::shouldReceivePowerFrom(LevelSource *level, int x, int y, int z, int direction)
{
    if (shouldConnectTo(level, x, y, z, direction))
	{
        return true;
    }

    int t = level->getTile(x, y, z);
    if (t == Tile::diode_on_Id)
	{
        int data = level->getData(x, y, z);
        return direction == (data & DiodeTile::DIRECTION_MASK);
    }
    return false;
}

int  RedStoneDustTile::cloneTileId(Level *level, int x, int y, int z)
{
	return Item::redStone_Id;
}

void RedStoneDustTile::registerIcons(IconRegister *iconRegister)
{
	iconCross = iconRegister->registerIcon(TEXTURE_CROSS);
	iconLine = iconRegister->registerIcon(TEXTURE_LINE);
	iconCrossOver = iconRegister->registerIcon(TEXTURE_CROSS_OVERLAY);
	iconLineOver = iconRegister->registerIcon(TEXTURE_LINE_OVERLAY);

	icon = iconCross;
}

Icon *RedStoneDustTile::getTexture(const wstring &name) 
{

	if (name.compare(TEXTURE_CROSS) == 0) return Tile::redStoneDust->iconCross;
	if (name.compare(TEXTURE_LINE) == 0) return Tile::redStoneDust->iconLine;
	if (name.compare(TEXTURE_CROSS_OVERLAY) == 0) return Tile::redStoneDust->iconCrossOver;
	if (name.compare(TEXTURE_LINE_OVERLAY) == 0) return Tile::redStoneDust->iconLineOver;
	return NULL;
}
