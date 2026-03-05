#pragma once
#include "Tile_SPU.h"

class LeafTile_SPU : public Tile_SPU
{
	friend class Tile;
public:
	static const int REQUIRED_WOOD_RANGE = 4;
    static const int UPDATE_LEAF_BIT = 8;
	static const int PERSISTENT_LEAF_BIT = 4;	// player-placed
    static const int NORMAL_LEAF = 0;
    static const int EVERGREEN_LEAF = 1;
    static const int BIRCH_LEAF = 2;
	static const int JUNGLE_LEAF = 3;

	static const int LEAF_NAMES_LENGTH = 3;

	static const int LEAF_TYPE_MASK = 3;

    // pppppppppp ppppppppppp pppppppppp ppppppp
    // ssssssssss sssssssssss s

	LeafTile_SPU(int id) : Tile_SPU(id) {}
public:
	virtual bool isSolidRender(bool isServerLevel = false);
	virtual bool shouldRenderFace(ChunkRebuildData *level, int x, int y, int z, int face); // from TransparentTile, since we're no longer inheriting
	virtual bool blocksLight() { return false; }


// 	virtual int getColor() const;
// 	virtual int getColor(int data);
    virtual int getColor(ChunkRebuildData *level, int x, int y, int z);
	virtual int getColor(ChunkRebuildData *level, int x, int y, int z, int data);	// 4J added

public:
    virtual Icon_SPU *getTexture(int face, int data);
    static void setFancy(bool fancyGraphics);
};