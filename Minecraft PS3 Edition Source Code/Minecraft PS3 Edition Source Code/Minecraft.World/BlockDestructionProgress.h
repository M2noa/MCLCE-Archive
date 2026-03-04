#pragma once

class BlockDestructionProgress
{
private:
	int id;
	int x;
	int y;
	int z;
	int progress;
	int updatedRenderTick;

public:
	BlockDestructionProgress(int id, int x, int y, int z);

	int getId();
	int getX();
	int getY();
	int getZ();
	void setProgress(int progress);
	int getProgress();
	void updateTick(int tick);
	int getUpdatedRenderTick();
};