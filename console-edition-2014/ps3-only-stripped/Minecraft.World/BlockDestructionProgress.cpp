#include "stdafx.h"
#include "BlockDestructionProgress.h"

BlockDestructionProgress::BlockDestructionProgress(int id, int x, int y, int z)
{
	this->id = id;
	this->x = x;
	this->y = y;
	this->z = z;

	progress = 0;
	updatedRenderTick = 0;
}

int BlockDestructionProgress::getId()
{
	return id;
}

int BlockDestructionProgress::getX()
{
	return x;
}

int BlockDestructionProgress::getY()
{
	return y;
}

int BlockDestructionProgress::getZ()
{
	return z;
}

void BlockDestructionProgress::setProgress(int progress)
{
	if (progress > 10)
	{
		progress = 10;
	}
	this->progress = progress;
}

int BlockDestructionProgress::getProgress()
{
	return progress;
}

void BlockDestructionProgress::updateTick(int tick)
{
	this->updatedRenderTick = tick;
}

int BlockDestructionProgress::getUpdatedRenderTick()
{
	return updatedRenderTick;
}