#pragma once
#include "StructurePiece.h"

class StrongholdPieces
{

private:
	static const int SMALL_DOOR_WIDTH = 3;
    static const int SMALL_DOOR_HEIGHT = 3;

    static const int MAX_DEPTH = 50;
    // the dungeon starts at 64 and traverses downwards to this point
    static const int LOWEST_Y_POSITION = 10;
	static const bool CHECK_AIR;

	// 4J - added to replace use of Class<? extends StrongholdPiece> within this class
	enum EPieceClass
	{
		EPieceClass_NULL,
		EPieceClass_Straight,
		EPieceClass_PrisonHall,
		EPieceClass_LeftTurn,
		EPieceClass_RightTurn,
		EPieceClass_RoomCrossing,
		EPieceClass_StraightStairsDown,
		EPieceClass_StairsDown,
		EPieceClass_FiveCrossing,
		EPieceClass_ChestCorridor,
		EPieceClass_Library,
		EPieceClass_PortalRoom
	};

	class PieceWeight
	{
	public:
		EPieceClass pieceClass;		// 4J - was Class<? extends StrongholdPiece>
		const int weight;
		int placeCount;
		int maxPlaceCount;
		
		PieceWeight(EPieceClass pieceClass, int weight, int maxPlaceCount);
		virtual bool doPlace(int depth);
		bool isValid();
    };

	// 4J - added, java uses a local specialisation of these classes when instancing to achieve the same thing
	class PieceWeight_Library : public PieceWeight
	{
	public:
		PieceWeight_Library(EPieceClass pieceClass, int weight, int maxPlaceCount) : PieceWeight(pieceClass, weight, maxPlaceCount) {}
		virtual bool doPlace(int depth) { return PieceWeight::doPlace(depth) && depth > 4; }
	};

	class PieceWeight_PortalRoom : public PieceWeight
	{
	public:
		PieceWeight_PortalRoom(EPieceClass pieceClass, int weight, int maxPlaceCount) : PieceWeight(pieceClass, weight, maxPlaceCount) {}
		virtual bool doPlace(int depth) { return PieceWeight::doPlace(depth) && depth > 5; }
	};

    static list<PieceWeight *> currentPieces;
	static EPieceClass imposedPiece;
    static int totalWeight;

public:
	static void resetPieces();
	class StartPiece;
private:
	class StrongholdPiece;
	static bool updatePieceWeight();
	static StrongholdPiece *findAndCreatePieceFactory(EPieceClass pieceClass, list<StructurePiece*> *pieces, Random *random, int footX, int footY, int footZ, int direction, int depth);
	static StrongholdPiece *generatePieceFromSmallDoor(StartPiece *startPiece, list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int depth);
	static StructurePiece *generateAndAddPiece(StartPiece *startPiece, list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int depth);


    /**
     * 
     *
     */
private:
	class StrongholdPiece : public StructurePiece
	{

	protected:
		StrongholdPiece(int genDepth);

        enum SmallDoorType
		{
            OPENING, WOOD_DOOR, GRATES, IRON_DOOR,
        };

		void generateSmallDoor(Level *level, Random *random, BoundingBox *chunkBB, SmallDoorType doorType, int footX, int footY, int footZ);
		SmallDoorType randomSmallDoor(Random *random);		
		StructurePiece *generateSmallDoorChildForward(StartPiece *startPiece, list<StructurePiece *> *pieces, Random *random, int xOff, int yOff);
		StructurePiece *generateSmallDoorChildLeft(StartPiece *startPiece, list<StructurePiece *> *pieces, Random *random, int yOff, int zOff);
		StructurePiece *generateSmallDoorChildRight(StartPiece *startPiece, list<StructurePiece *> *pieces, Random *random, int yOff, int zOff);
		
		static bool isOkBox(BoundingBox *box, StartPiece *startRoom); // 4J added startRoom param
    };

    /**
     * Corridor pieces that connects unconnected ends.
     * 
     */
public:
	class FillerCorridor : public StrongholdPiece
	{
	private:
		const int steps;

	public:
		FillerCorridor(int genDepth, Random *random, BoundingBox *corridorBox, int direction);

        static BoundingBox *findPieceBox(list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction);
		virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);
    };

    /**
     * 
     *
     */
public:
	class StairsDown : public StrongholdPiece
	{
	private:
		static const int width = 5;
		static const int height = 11;
		static const int depth = 5;
		
		const bool isSource;
		const SmallDoorType entryDoor;

	public:
		StairsDown(int genDepth, Random *random, int west, int north);
		StairsDown(int genDepth, Random *random, BoundingBox *stairsBox, int direction);

        virtual void addChildren(StructurePiece *startPiece, list<StructurePiece *> *pieces, Random *random);
        static StairsDown *createPiece(list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int genDepth);
        virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);
    };

public:
	class PortalRoom;

	class StartPiece : public StairsDown
	{
	public:
		bool isLibraryAdded;
        PieceWeight *previousPiece;
		PortalRoom *portalRoomPiece;
		Level *m_level; // 4J added

        // this queue is used so that the addChildren calls are
        // called in a random order
        vector<StructurePiece *> pendingChildren;

        StartPiece(int genDepth, Random *random, int west, int north, Level *level); // 4J Added level param
		virtual TilePos *getLocatorPosition();
    };

    /**
     * 
     *
     */
public:
	class Straight : public StrongholdPiece
	{
	private:
		static const int width = 5;
		static const int height = 5;
		static const int depth = 7;

		const SmallDoorType entryDoor;
		const bool leftChild;
		const bool rightChild;

	public:
		Straight(int genDepth, Random *random, BoundingBox *stairsBox, int direction);
		virtual void addChildren(StructurePiece *startPiece, list<StructurePiece *> *pieces, Random *random);
		static Straight *createPiece(list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int genDepth);
		virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);

    };

   /**
     * 
     *
     */

    class ChestCorridor : public StrongholdPiece
	{
	private:
		static const int width = 5;
        static const int height = 5;
        static const int depth = 7;
		static const int TREASURE_ITEMS_COUNT = 14;
		static WeighedTreasure *treasureItems[TREASURE_ITEMS_COUNT];

        const SmallDoorType entryDoor;
        boolean hasPlacedChest;

	public:
		ChestCorridor(int genDepth, Random *random, BoundingBox *stairsBox, int direction);
        virtual void addChildren(StructurePiece *startPiece, list<StructurePiece *> *pieces, Random *random);
        static ChestCorridor *createPiece(list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int genDepth);
        virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);
    };

    /**
     * 
     *
     */
public:
	class StraightStairsDown : public StrongholdPiece
	{
	private:
		static const int width = 5;
		static const int height = 11;
		static const int depth = 8;

		const SmallDoorType entryDoor;

	public:
		StraightStairsDown(int genDepth, Random *random, BoundingBox *stairsBox, int direction);
		virtual void addChildren(StructurePiece *startPiece, list<StructurePiece *> *pieces, Random *random);
		static StraightStairsDown *createPiece(list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int genDepth);
		virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);
    };

    /**
     * 
     *
     */
public:
	class LeftTurn : public StrongholdPiece
	{

	protected:
		static const int width = 5;
		static const int height = 5;
		static const int depth = 5;

		const SmallDoorType entryDoor;

	public:
		LeftTurn(int genDepth, Random *random, BoundingBox *stairsBox, int direction);
		virtual void addChildren(StructurePiece *startPiece, list<StructurePiece *> *pieces, Random *random);
		static LeftTurn *createPiece(list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int genDepth);
		virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);
    };

    /**
     * 
     *
     */
public:
	class RightTurn : public LeftTurn
	{
	public:
		RightTurn(int genDepth, Random *random, BoundingBox *stairsBox, int direction);
		virtual void addChildren(StructurePiece *startPiece, list<StructurePiece *> *pieces, Random *random);
	    virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);
	};

    /**
     * 
     *
     */
public:
	class RoomCrossing : public StrongholdPiece
	{
	private:
		static const int SMALL_TREASURE_ITEMS_COUNT = 7;	// 4J added
		static WeighedTreasure *smallTreasureItems[SMALL_TREASURE_ITEMS_COUNT];

	protected:
		static const int width = 11;
		static const int height = 7;
		static const int depth = 11;

	protected:
		const SmallDoorType entryDoor;
        const int type;
	public:
		RoomCrossing(int genDepth, Random *random, BoundingBox *stairsBox, int direction);
        virtual void addChildren(StructurePiece *startPiece, list<StructurePiece *> *pieces, Random *random);
        static RoomCrossing *createPiece(list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int genDepth);
        virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);
    };

    /**
     * 
     *
     */
public:
	class PrisonHall : public StrongholdPiece
	{
	protected:
		static const int width = 9;
		static const int height = 5;
		static const int depth = 11;

        const SmallDoorType entryDoor;

	public:
		PrisonHall(int genDepth, Random *random, BoundingBox *stairsBox, int direction);
        virtual void addChildren(StructurePiece *startPiece, list<StructurePiece *> *pieces, Random *random);
		static PrisonHall *createPiece(list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int genDepth);
        virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);
    };

    /**
     * 
     *
     */
public:
	class Library : public StrongholdPiece
	{
	private:
		static const int LIBRARY_TREASURE_ITEMS_COUNT = 4;	// 4J added
		static WeighedTreasure *libraryTreasureItems[LIBRARY_TREASURE_ITEMS_COUNT];

	protected:
		static const int width = 14;
		static const int height = 6;
		static const int tallHeight = 11;
		static const int depth = 15;

        const SmallDoorType entryDoor;
	private:
		const bool isTall;

	public:
		Library(int genDepth, Random *random, BoundingBox *roomBox, int direction);
        static Library *createPiece(list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int genDepth);
        virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);

    };

    /**
     * 
     *
     */
public:
	class FiveCrossing : public StrongholdPiece
	{
	protected:
		static const int width = 10;
        static const int height = 9;
        static const int depth = 11;

        const SmallDoorType entryDoor;

	private:
		bool leftLow, leftHigh, rightLow, rightHigh;

	public:
		FiveCrossing(int genDepth, Random *random, BoundingBox *stairsBox, int direction);
        virtual void addChildren(StructurePiece *startPiece, list<StructurePiece *> *pieces, Random *random);
        static FiveCrossing *createPiece(list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int genDepth);
        virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);
    };

    /**
     * 
     *
     */

    class PortalRoom : public StrongholdPiece
	{
	protected:
		static const int width = 11;
        static const int height = 8;
        static const int depth = 16;

	private:
		bool hasPlacedMobSpawner;

	public:
		PortalRoom(int genDepth, Random *random, BoundingBox *stairsBox, int direction);
		void addChildren(StructurePiece *startPiece, list<StructurePiece *> *pieces, Random *random);
		static PortalRoom *createPiece(list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int genDepth);
		bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);
    };

private:

	class SmoothStoneSelector : public StructurePiece::BlockSelector
	{
	public:
		virtual void next(Random *random, int worldX, int worldY, int worldZ, bool isEdge);
    };

    static const SmoothStoneSelector *smoothStoneSelector;
};
