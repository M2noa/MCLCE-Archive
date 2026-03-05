#pragma once

class VillageSiege
{
private:
	Level *level;
	bool hasSetupSiege;
	int siegeState;
	int siegeCount;
	int nextSpawnTime;
	weak_ptr<Village> village;
	int spawnX, spawnY, spawnZ;

	static const int SIEGE_NOT_INITED = -1;
	static const int SIEGE_CAN_ACTIVATE = 0;
	static const int SIEGE_TONIGHT = 1;
	static const int SIEGE_DONE = 2;

public:
	VillageSiege(Level *level);

	void tick();

private:
	bool tryToSetupSiege();
	bool trySpawn();
	Vec3 *findRandomSpawnPos(int x, int y, int z);
};