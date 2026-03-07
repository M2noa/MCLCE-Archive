#pragma once

#include "StructurePiece.h"

class MineShaftPieces
{
private:
	static const int DEFAULT_SHAFT_WIDTH = 3;
	static const int DEFAULT_SHAFT_HEIGHT = 3;
	static const int DEFAULT_SHAFT_LENGTH = 5;

	static const int MAX_DEPTH = 8; // 1.2.3 change

	static StructurePiece *createRandomShaftPiece(list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int genDepth);
	static StructurePiece *generateAndAddPiece(StructurePiece *startPiece, list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int depth);

	/**
	* 
	*
	*/
public:
	class MineShaftRoom : public StructurePiece
	{
	private:
		list<BoundingBox *> childEntranceBoxes;

	public:
		MineShaftRoom(int genDepth, Random *random, int west, int north);
		~MineShaftRoom();

		virtual void addChildren(StructurePiece *startPiece, list<StructurePiece *> *pieces, Random *random);
		virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);
	};

	/**
	* 
	*
	*/
	class MineShaftCorridor : public StructurePiece
	{
	private:
		bool hasRails; // was final
		bool spiderCorridor; // was final
		bool hasPlacedSpider;
		int numSections;

	public:
		MineShaftCorridor(int genDepth, Random *random, BoundingBox *corridorBox, int direction);

		static BoundingBox *findCorridorSize(list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction);
		virtual void addChildren(StructurePiece *startPiece, list<StructurePiece *> *pieces, Random *random);
		virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);
	};

	/**
	* 
	*
	*/
	class MineShaftCrossing : public StructurePiece
	{
	private:
		const int direction;
		const bool isTwoFloored;

	public:
		MineShaftCrossing(int genDepth, Random *random, BoundingBox *crossingBox, int direction);

		static BoundingBox *findCrossing(list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction);
		virtual void addChildren(StructurePiece *startPiece, list<StructurePiece *> *pieces, Random *random);
		virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);
	};

	/**
	* 
	*
	*/
	class MineShaftStairs : public StructurePiece
	{
	public:
		MineShaftStairs(int genDepth, Random *random, BoundingBox *stairsBox, int direction);

		static BoundingBox *findStairs(list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction);
		virtual void addChildren(StructurePiece *startPiece, list<StructurePiece *> *pieces, Random *random);
		virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);

	};

	/* @formatter:off */
private:
	static WeighedTreasureArray smallTreasureItems;
	/* @formatter:on */

public:
	static void staticCtor();

};