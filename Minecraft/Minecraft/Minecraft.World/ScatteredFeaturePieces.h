#pragma once

#include "StructurePiece.h"

class ScatteredFeaturePieces
{
private:
	class ScatteredFeaturePiece : public StructurePiece
	{
	protected:
		int width;
		int height;
		int depth;

		int heightPosition;

		ScatteredFeaturePiece(Random *random, int west, int floor, int north, int width, int height, int depth);

		bool updateAverageGroundHeight(Level *level, BoundingBox *chunkBB, int offset);
	};

public:
	class DesertPyramidPiece : public ScatteredFeaturePiece
	{
	public:
		static const int TREASURE_ITEMS_COUNT = 6;
	private:
		bool hasPlacedChest[4];
		static WeighedTreasure *treasureItems[TREASURE_ITEMS_COUNT];

	public:
		DesertPyramidPiece(Random *random, int west, int north);

		bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);

	};

	class JunglePyramidPiece : public ScatteredFeaturePiece
	{
	public:
		static const int TREASURE_ITEMS_COUNT = 6;
		static const int DISPENSER_ITEMS_COUNT = 1;
	private:
		bool placedMainChest;
		bool placedHiddenChest;
		bool placedTrap1;
		bool placedTrap2;

		static WeighedTreasure *treasureItems[TREASURE_ITEMS_COUNT];
		static WeighedTreasure *dispenserItems[DISPENSER_ITEMS_COUNT];

	public:
		JunglePyramidPiece(Random *random, int west, int north);

		bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);

	private:
		class MossStoneSelector : public BlockSelector
		{
		public:
			void next(Random *random, int worldX, int worldY, int worldZ, bool isEdge);
		};

		static MossStoneSelector stoneSelector;

	};
};