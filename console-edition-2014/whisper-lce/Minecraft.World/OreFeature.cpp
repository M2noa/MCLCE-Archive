#include "stdafx.h"
#include "net.minecraft.world.level.h"
#include "net.minecraft.world.level.tile.h"
#include "OreFeature.h"

void OreFeature::_init(int tile, int count, int targetTile)
{
	this->tile = tile;
	this->count = count;
	this->targetTile = targetTile;
}

OreFeature::OreFeature(int tile, int count)
{
	_init(tile, count, Tile::rock_Id);
}

OreFeature::OreFeature(int tile, int count, int targetTile)
{
	_init(tile, count, targetTile);
}

bool OreFeature::place(Level *level, Random *random, int x, int y, int z)
{
	PIXBeginNamedEvent(0,"Place Ore Feature");
    float dir = random->nextFloat() * PI;

    double x0 = x + 8 + Mth::sin(dir) * count / 8;
    double x1 = x + 8 - Mth::sin(dir) * count / 8;
    double z0 = z + 8 + Mth::cos(dir) * count / 8;
    double z1 = z + 8 - Mth::cos(dir) * count / 8;

    double y0 = y + random->nextInt(3) - 2;
    double y1 = y + random->nextInt(3) - 2;

	bool collisionsExpected = false;
	
	LevelGenerationOptions *levelGenOptions = NULL;
	if( app.getLevelGenerationOptions() != NULL )
	{
		levelGenOptions = app.getLevelGenerationOptions();

		// 4J Stu - Optimise schematic intersection checks by first checking the max possible bounding box of this place call
		int minX = x0 - 1;
		int minY = y0 - 1;
		int minZ = z0 - 1;
	
		double maxss = count / 16;
		double maxr = (Mth::sin(PI) + 1) * maxss + 1;
		double maxhr = (Mth::sin(PI) + 1) * maxss + 1;
		int maxX = Mth::floor(x1 + maxr / 2);
		int maxY = Mth::floor(y1 + maxhr / 2);
		int maxZ = Mth::floor(z1 + maxr / 2);

		collisionsExpected = levelGenOptions->checkIntersects(minX, minY, minZ, maxX, maxY, maxZ);
	}

    for (int d = 0; d <= count; d++)
	{
        double xx = x0 + (x1 - x0) * d / count;
        double yy = y0 + (y1 - y0) * d / count;
        double zz = z0 + (z1 - z0) * d / count;

        double ss = random->nextDouble() * count / 16;
        double r = (Mth::sin(d * PI / count) + 1) * ss + 1;
        double hr = (Mth::sin(d * PI / count) + 1) * ss + 1;

        int xt0 = Mth::floor(xx - r / 2);
        int yt0 = Mth::floor(yy - hr / 2);
        int zt0 = Mth::floor(zz - r / 2);
				  
        int xt1 = Mth::floor(xx + r / 2);
        int yt1 = Mth::floor(yy + hr / 2);
        int zt1 = Mth::floor(zz + r / 2);

		// 4J Stu Added to stop ore features generating areas previously place by game rule generation
		if(collisionsExpected && levelGenOptions != NULL)
		{
			bool intersects = levelGenOptions->checkIntersects(xt0, yt0, zt0, xt1, yt1, zt1);
			if(intersects)
			{
				//app.DebugPrintf("Skipping ore feature generation as it overlaps a game rule structure\n");
				continue;
			}
		}

		// A large % of ore placement is entirely into the air. Attempt to identify some of these early, by check the corners
		// of the area we are placing in to see if we are going to (very probably) be entirely above the height stored in the heightmap

		bool earlyReject = true;
		if     ( level->getHeightmap(xt0, zt0) >= yt0 ) earlyReject = false;
		else if( level->getHeightmap(xt1, zt0) >= yt0 ) earlyReject = false;
		else if( level->getHeightmap(xt0, zt1) >= yt0 ) earlyReject = false;
		else if( level->getHeightmap(xt1, zt1) >= yt0 ) earlyReject = false;

		if( earlyReject ) continue;

        for (int x2 = xt0; x2 <= xt1; x2++)
		{
            double xd = ((x2 + 0.5) - xx) / (r / 2);
            if (xd * xd < 1)
			{
                for (int y2 = yt0; y2 <= yt1; y2++)
				{
                    double yd = ((y2 + 0.5) - yy) / (hr / 2);
                    if (xd * xd + yd * yd < 1)
					{
                        for (int z2 = zt0; z2 <= zt1; z2++)
						{
                            double zd = ((z2 + 0.5) - zz) / (r / 2);
                            if (xd * xd + yd * yd + zd * zd < 1)
							{
                                if (level->getTile(x2, y2, z2) == targetTile)
								{
									level->setTileNoUpdateNoLightCheck(x2, y2, z2, tile);		// 4J changed from setTileNoUpdate
								}
                            }
                        }
                    }
                }
            }
        }
    }

	PIXEndNamedEvent();
    return true;
}