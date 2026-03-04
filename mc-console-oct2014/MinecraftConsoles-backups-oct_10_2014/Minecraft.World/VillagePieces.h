#pragma once
#include "StructurePiece.h"

class BiomeSource;

class VillagePieces
{

private:
	static const int MAX_DEPTH = 50;
    static const int BASE_ROAD_DEPTH = 3;
    // the dungeon starts at 64 and traverses downwards to this point
    static const int LOWEST_Y_POSITION = 10;

public:
	static const int SIZE_SMALL = 0;
	static const int SIZE_BIG = 1;
	static const int SIZE_BIGGEST = 2;

	// 4J - added to replace use of Class<? extends VillagePiece> within this class
	enum EPieceClass
	{
		EPieceClass_SimpleHouse,
		EPieceClass_SmallTemple,
		EPieceClass_BookHouse,
		EPieceClass_SmallHut,
		EPieceClass_PigHouse,
		EPieceClass_DoubleFarmland,
		EPieceClass_Farmland,
		EPieceClass_Smithy,
		EPieceClass_TwoRoomHouse
	};

    class PieceWeight {
	public:
		EPieceClass pieceClass;		// 4J - EPieceClass was Class<? extends VillagePiece>
        const int weight;
        int placeCount;
        int maxPlaceCount;

        PieceWeight(EPieceClass pieceClass, int weight, int maxPlaceCount);	// 4J - EPieceClass was Class<? extends VillagePiece>
        bool doPlace(int depth);
        bool isValid();
    };

    static list<PieceWeight *> *createPieceSet(Random *random, int villageSize);		// 4J - was ArrayList

	class StartPiece;
private:
	class VillagePiece;
	static int updatePieceWeight(list<PieceWeight *> *currentPieces);		// 4J = was array list
	static VillagePiece *findAndCreatePieceFactory(StartPiece *startPiece, PieceWeight *piece, list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int depth);
	static VillagePiece *generatePieceFromSmallDoor(StartPiece *startPiece, list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int depth);
	static StructurePiece *generateAndAddPiece(StartPiece *startPiece, list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int depth);
	static StructurePiece *generateAndAddRoadPiece(StartPiece *startPiece, list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int depth);


    /**
     * 
     *
     */
private:
	class VillagePiece : public StructurePiece
	{
	private:
		int spawnedVillagerCount;
	protected:
		StartPiece *startPiece;

		VillagePiece(StartPiece *startPiece, int genDepth);
		StructurePiece *generateHouseNorthernLeft(StartPiece *startPiece, list<StructurePiece *> *pieces, Random *random, int yOff, int zOff);
		StructurePiece *generateHouseNorthernRight(StartPiece *startPiece, list<StructurePiece *> *pieces, Random *random, int yOff, int zOff);
		int getAverageGroundHeight(Level *level, BoundingBox *chunkBB);
		static bool isOkBox(BoundingBox *box, StartPiece *startRoom); // 4J added startRoom param
		void spawnVillagers(Level *level, BoundingBox *chunkBB, int x, int y, int z, int count);
		virtual int getVillagerProfession(int villagerNumber);
		virtual int biomeBlock(int tile, int data);
		virtual int biomeData(int tile, int data);
		virtual void placeBlock(Level *level, int block, int data, int x, int y, int z, BoundingBox *chunkBB);
		virtual void generateBox(Level *level, BoundingBox *chunkBB, int x0, int y0, int z0, int x1, int y1, int z1, int edgeTile, int fillTile, bool skipAir);
		virtual void fillColumnDown(Level *level, int block, int data, int x, int startY, int z, BoundingBox *chunkBB);
    };

    /**
     * 
     *
     */
public:
	class Well : public VillagePiece
	{
	private:
		static const int width = 6;
		static const int height = 15;
		static const int depth = 6;
		
		const bool isSource;
		int heightPosition;

	public:
		Well(StartPiece *startPiece, int genDepth, Random *random, int west, int north);

        Well(StartPiece *startPiece, int genDepth, Random *random, BoundingBox *stairsBox, int direction);
        virtual void addChildren(StructurePiece *startPiece, list<StructurePiece *> *pieces, Random *random);
        //static Well *createPiece(list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int genDepth);
        virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);
    };

public:
	class StartPiece : public Well
	{
	public:
		BiomeSource *biomeSource;
		bool isDesertVillage;

		int villageSize;
		bool isLibraryAdded;
		PieceWeight *previousPiece;
		list<PieceWeight *> *pieceSet;		// 4J - was ArrayList
		Level *m_level;

        // these queues are used so that the addChildren calls are
        // called in a random order
		vector<StructurePiece *> pendingHouses;		// 4J - was ArrayList
		vector<StructurePiece *> pendingRoads;		// 4J - was ArrayList
		
		StartPiece(BiomeSource *biomeSource, int genDepth, Random *random, int west, int north, list<PieceWeight *> *pieceSet, int villageSize, Level *level); // 4J Added level param
		virtual ~StartPiece();
		
		BiomeSource *getBiomeSource();

    };

public:
	class VillageRoadPiece : public VillagePiece
	{
	protected :
		VillageRoadPiece(StartPiece *startPiece, int genDepth) : VillagePiece(startPiece, genDepth) {}
    };

    /**
     * 
     *
     */
public:
	class StraightRoad : public VillageRoadPiece
	{

	private:
		static const int width = 3;
		int length;
	public:
		StraightRoad(StartPiece *startPiece, int genDepth, Random *random, BoundingBox *stairsBox, int direction);
		virtual void addChildren(StructurePiece *startPiece, list<StructurePiece *> *pieces, Random *random);
		static BoundingBox *findPieceBox(StartPiece *startPiece, list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction);
		virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);
    };

    /**
     * 
     *
     */
public:
	class SimpleHouse : public VillagePiece
	{
	private:
		static const int width = 5;
		static const int height = 6;
		static const int depth = 5;

	private:
		int heightPosition;
		const bool hasTerrace;

	public:
		SimpleHouse(StartPiece *startPiece, int genDepth, Random *random, BoundingBox *stairsBox, int direction);
	public:
		static SimpleHouse *createPiece(StartPiece *startPiece, list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int genDepth);
    virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);
	};

public:
	class SmallTemple : public VillagePiece
	{
	private:
		static const int width = 5;
		static const int height = 12;
		static const int depth = 9;

		int heightPosition;

	public:
		SmallTemple(StartPiece *startPiece, int genDepth, Random *random, BoundingBox *stairsBox, int direction);

		static SmallTemple *createPiece(StartPiece *startPiece, list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int genDepth);
		virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);
		virtual int getVillagerProfession(int villagerNumber);
	};

public:
	class BookHouse : public VillagePiece
	{
	private:
		static const int width = 9;
		static const int height = 9;
		static const int depth = 6;

		int heightPosition;

	public:
		BookHouse(StartPiece *startPiece, int genDepth, Random *random, BoundingBox *stairsBox, int direction);

		static BookHouse *createPiece(StartPiece *startPiece, list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int genDepth);
		virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);
		virtual int getVillagerProfession(int villagerNumber);
	};

	public:
		class SmallHut : public VillagePiece
		{

		private:
			static const int width = 4;
			static const int height = 6;
			static const int depth = 5;

			int heightPosition;
			const bool lowCeiling;
			const int tablePlacement;

		public:
			SmallHut(StartPiece *startPiece, int genDepth, Random *random, BoundingBox *stairsBox, int direction);
			static SmallHut *createPiece(StartPiece *startPiece, list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int genDepth);
	        virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);
    };

public:
	class PigHouse : public VillagePiece
	{

	private:
		static const int width = 9;
		static const int height = 7;
		static const int depth = 11;

		int heightPosition;

	public:
		PigHouse(StartPiece *startPiece, int genDepth, Random *random, BoundingBox *stairsBox, int direction);
		static PigHouse *createPiece(StartPiece *startPiece, list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int genDepth);
		virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);
		virtual int getVillagerProfession(int villagerNumber);
	};

public:
	class TwoRoomHouse : public VillagePiece
	{
	private:
		static const int width = 9;
		static const int height = 7;
		static const int depth = 12;

		int heightPosition;

	public:
		TwoRoomHouse(StartPiece *startPiece, int genDepth, Random *random, BoundingBox *stairsBox, int direction);
		static TwoRoomHouse *createPiece(StartPiece *startPiece, list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int genDepth);
		virtual  bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);
	};

public:
    class Smithy : public VillagePiece
	{

	private:
		static const int width = 10;
		static const int height = 6;
		static const int depth = 7;

        int heightPosition;
		bool hasPlacedChest;

		static WeighedTreasureArray treasureItems;

	public:
		static void staticCtor();

		Smithy(StartPiece *startPiece, int genDepth, Random *random, BoundingBox *stairsBox, int direction);
        static Smithy *createPiece(StartPiece *startPiece, list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int genDepth);
        virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);
		virtual int getVillagerProfession(int villagerNumber);
    };

public:
	class Farmland : public VillagePiece
	{

	private:
		static const int width = 7;
		static const int height = 4;
		static const int depth = 9;
		
		int heightPosition;

		int cropsA;
		int cropsB;

		int selectCrops(Random *random);

	public:
		Farmland(StartPiece *startPiece, int genDepth, Random *random, BoundingBox *stairsBox, int direction);
        static Farmland *createPiece(StartPiece *startPiece, list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int genDepth);
		virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);
    };

public:
	class DoubleFarmland : public VillagePiece
	{
	private:
		static const int width = 13;
		static const int height = 4;
		static const int depth = 9;
		
		int heightPosition;

		int cropsA;
		int cropsB;
		int cropsC;
		int cropsD;

		int selectCrops(Random *random);

	public:
		DoubleFarmland(StartPiece *startPiece, int genDepth, Random *random, BoundingBox *stairsBox, int direction);
        static DoubleFarmland *createPiece(StartPiece *startPiece, list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int genDepth);
		virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);
    };

public:
	class LightPost : public VillagePiece
	{
	private:
		static const int width = 3;
        static const int height = 4;
        static const int depth = 2;

        int heightPosition;

	public:
		LightPost(StartPiece *startPiece, int genDepth, Random *random, BoundingBox *box, int direction);
        static BoundingBox *findPieceBox(StartPiece *startPiece, list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction);
        virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);
    };
};
