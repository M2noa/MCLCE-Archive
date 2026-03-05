#pragma once
#include "StructurePiece.h"

class NetherBridgePieces
{
private:
	static const int MAX_DEPTH = 30;
    // the dungeon starts at 64 and traverses downwards to this point
    static const int LOWEST_Y_POSITION = 10;

	// 4J - added to replace use of Class<? extends NetherBridgePiece> within this class
	enum EPieceClass
	{
		EPieceClass_BridgeStraight,
		EPieceClass_BridgeEndFiller,
		EPieceClass_BridgeCrossing,
		EPieceClass_RoomCrossing,
		EPieceClass_StairsRoom,
		EPieceClass_MonsterThrone,
		EPieceClass_CastleEntrance,
		EPieceClass_CastleStalkRoom,
		EPieceClass_CastleSmallCorridorPiece,
		EPieceClass_CastleSmallCorridorCrossingPiece,
		EPieceClass_CastleSmallCorridorRightTurnPiece,
		EPieceClass_CastleSmallCorridorLeftTurnPiece,
		EPieceClass_CastleCorridorStairsPiece,
		EPieceClass_CastleCorridorTBalconyPiece
	};

    class PieceWeight
	{
	public:
        EPieceClass pieceClass;
        const int weight;
        int placeCount;
        int maxPlaceCount;
        bool allowInRow;

        PieceWeight(EPieceClass pieceClass, int weight, int maxPlaceCount, bool allowInRow);
        PieceWeight(EPieceClass pieceClass, int weight, int maxPlaceCount);
        bool doPlace(int depth);
        bool isValid();
    };

	static const int BRIDGE_PIECEWEIGHTS_COUNT = 6;
	static const int CASTLE_PIECEWEIGHTS_COUNT = 7;
    static NetherBridgePieces::PieceWeight *bridgePieceWeights[BRIDGE_PIECEWEIGHTS_COUNT];
	static NetherBridgePieces::PieceWeight *castlePieceWeights[CASTLE_PIECEWEIGHTS_COUNT];

private:
	class NetherBridgePiece;
	static NetherBridgePiece *findAndCreateBridgePieceFactory(NetherBridgePieces::PieceWeight *piece, list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int depth);

    /**
     * 
     *
     */
public:
	class StartPiece;
private:

    class NetherBridgePiece : public StructurePiece
	{
		protected:
				  NetherBridgePiece(int genDepth);
	private:
		int updatePieceWeight(list<PieceWeight *> *currentPieces);

        NetherBridgePiece *generatePiece(StartPiece *startPiece, list<NetherBridgePieces::PieceWeight *> *currentPieces, list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int depth);
        StructurePiece *generateAndAddPiece(StartPiece *startPiece, list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int depth, bool isCastle);
	protected:
		StructurePiece *generateChildForward(StartPiece *startPiece, list<StructurePiece *> *pieces, Random *random, int xOff, int yOff, bool isCastle);
		StructurePiece *generateChildLeft(StartPiece *startPiece, list<StructurePiece *> *pieces, Random *random, int yOff, int zOff, bool isCastle);
		StructurePiece *generateChildRight(StartPiece *startPiece, list<StructurePiece *> *pieces, Random *random, int yOff, int zOff, bool isCastle);
		
		static bool isOkBox(BoundingBox *box, StartPiece *startRoom); // 4J added startRoom param
		void generateLightPost(Level *level, Random *random, BoundingBox *chunkBB, int x, int y, int z, int xOff, int zOff);
		
		void generateLightPostFacingRight(Level *level, Random *random, BoundingBox *chunkBB, int x, int y, int z);
		void generateLightPostFacingLeft(Level *level, Random *random, BoundingBox *chunkBB, int x, int y, int z);
		void generateLightPostFacingUp(Level *level, Random *random, BoundingBox *chunkBB, int x, int y, int z);
		void generateLightPostFacingDown(Level *level, Random *random, BoundingBox *chunkBB, int x, int y, int z);
    };

    /**
     * 
     *
     */
	class BridgeStraight : public NetherBridgePiece
	{
	private:
		static const int width = 5;
        static const int height = 10;
        static const int depth = 19;

	public:
		BridgeStraight(int genDepth, Random *random, BoundingBox *stairsBox, int direction);
        virtual void addChildren(StructurePiece *startPiece, list<StructurePiece *> *pieces, Random *random);
        static BridgeStraight *createPiece(list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int genDepth);
        virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);
    };

    class BridgeEndFiller : public NetherBridgePiece
	{
	private:
		static const int width = 5;
        static const int height = 10;
        static const int depth = 8;

        int selfSeed;

	public:
		BridgeEndFiller(int genDepth, Random *random, BoundingBox *stairsBox, int direction);
        static BridgeEndFiller *createPiece(list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int genDepth);
        virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);
    };

    class BridgeCrossing : public NetherBridgePiece
	{
	private:
		static const int width = 19;
        static const int height = 10;
        static const int depth = 19;

	public:
		BridgeCrossing(int genDepth, Random *random, BoundingBox *stairsBox, int direction);
	protected:
		BridgeCrossing(Random *random, int west, int north);
	public:
        virtual void addChildren(StructurePiece *startPiece, list<StructurePiece *> *pieces, Random *random);
        static BridgeCrossing *createPiece(list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int genDepth);
        virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);
    };

public:
    class StartPiece : public BridgeCrossing
	{

	public:
		bool isLibraryAdded;
        PieceWeight *previousPiece;
		Level *m_level;

        list<PieceWeight *> availableBridgePieces;
        list<PieceWeight *> availableCastlePieces;

        // this queue is used so that the addChildren calls are
        // called in a random order
        vector<StructurePiece *> pendingChildren;

        StartPiece(Random *random, int west, int north, Level *level); // 4J Added level param

    };

private:
    class RoomCrossing : public NetherBridgePiece
	{
	private:
		static const int width = 7;
        static const int height = 9;
        static const int depth = 7;

	public:
		RoomCrossing(int genDepth, Random *random, BoundingBox *box, int direction);
        virtual void addChildren(StructurePiece *startPiece, list<StructurePiece *> *pieces, Random *random);
        static RoomCrossing *createPiece(list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int genDepth);
        virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);
    };

    class StairsRoom : public NetherBridgePiece
	{
	private:
		static const int width = 7;
        static const int height = 11;
        static const int depth = 7;

	public:
		StairsRoom(int genDepth, Random *random, BoundingBox *box, int direction);
		virtual void addChildren(StructurePiece *startPiece, list<StructurePiece *> *pieces, Random *random);
        static StairsRoom *createPiece(list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int genDepth);
        virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);
    };

    class MonsterThrone : public NetherBridgePiece
	{
	private:
		static const int width = 7;
        static const int height = 8;
        static const int depth = 9;

        bool hasPlacedMobSpawner;

	public:
		MonsterThrone(int genDepth, Random *random, BoundingBox *box, int direction);
        static MonsterThrone *createPiece(list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int genDepth);
        virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);
    };

    /**
     * 
     *
     */
    class CastleEntrance : public NetherBridgePiece
	{
	private:
        static const int width = 13;
        static const int height = 14;
        static const int depth = 13;
	public:
        CastleEntrance(int genDepth, Random *random, BoundingBox *stairsBox, int direction);
        virtual void addChildren(StructurePiece *startPiece, list<StructurePiece *> *pieces, Random *random);
        static CastleEntrance *createPiece(list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int genDepth);
        virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);
    };

    /**
     * 
     *
     */
    class CastleStalkRoom : public NetherBridgePiece
	{
	private:
        static const int width = 13;
        static const int height = 14;
        static const int depth = 13;

	public:
		CastleStalkRoom(int genDepth, Random *random, BoundingBox *stairsBox, int direction);
        virtual void addChildren(StructurePiece *startPiece, list<StructurePiece *> *pieces, Random *random);
        static CastleStalkRoom *createPiece(list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int genDepth);
        virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);
    };

    /**
     * 
     *
     */
	class CastleSmallCorridorPiece : public NetherBridgePiece
	{
	private:
        static const int width = 5;
        static const int height = 7;
        static const int depth = 5;

	public:
		CastleSmallCorridorPiece(int genDepth, Random *random, BoundingBox *stairsBox, int direction);
        virtual void addChildren(StructurePiece *startPiece, list<StructurePiece *> *pieces, Random *random);
        static CastleSmallCorridorPiece *createPiece(list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int genDepth);
        virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);
    };

    /**
     * 
     *
     */
    class CastleSmallCorridorCrossingPiece : public NetherBridgePiece
	{
	private:
        static const int width = 5;
        static const int height = 7;
        static const int depth = 5;

	public:
		CastleSmallCorridorCrossingPiece(int genDepth, Random *random, BoundingBox *stairsBox, int direction);
        virtual void addChildren(StructurePiece *startPiece, list<StructurePiece *> *pieces, Random *random);
        static CastleSmallCorridorCrossingPiece *createPiece(list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int genDepth);
        virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);
    };

    /**
     * 
     *
     */
    class CastleSmallCorridorRightTurnPiece : public NetherBridgePiece
	{
	private:
		static const int width = 5;
		static const int height = 7;
		static const int depth = 5;

	public:
		CastleSmallCorridorRightTurnPiece(int genDepth, Random *random, BoundingBox *stairsBox, int direction);
        virtual void addChildren(StructurePiece *startPiece, list<StructurePiece *> *pieces, Random *random);
        static CastleSmallCorridorRightTurnPiece *createPiece(list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int genDepth);
        virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);

    };

    /**
     * 
     *
     */
    class CastleSmallCorridorLeftTurnPiece : public NetherBridgePiece
	{
	private:
		static const int width = 5;
		static const int height = 7;
		static const int depth = 5;

	public:
		CastleSmallCorridorLeftTurnPiece(int genDepth, Random *random, BoundingBox *stairsBox, int direction);
        virtual void addChildren(StructurePiece *startPiece, list<StructurePiece *> *pieces, Random *random);
        static CastleSmallCorridorLeftTurnPiece *createPiece(list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int genDepth);
        virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);
    };

    /**
     * 
     *
     */
    class CastleCorridorStairsPiece : public NetherBridgePiece
	{
	private:
		static const int width = 5;
		static const int height = 14;
		static const int depth = 10;

	public:
		CastleCorridorStairsPiece(int genDepth, Random *random, BoundingBox *stairsBox, int direction);
        virtual void addChildren(StructurePiece *startPiece, list<StructurePiece *> *pieces, Random *random);
        static CastleCorridorStairsPiece *createPiece(list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int genDepth);
        virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);

    };

    /**
     * 
     *
     */
    class CastleCorridorTBalconyPiece : public NetherBridgePiece
	{
	private:
        static const int width = 9;
        static const int height = 7;
        static const int depth = 9;

	public:
		CastleCorridorTBalconyPiece(int genDepth, Random *random, BoundingBox *stairsBox, int direction);
        virtual void addChildren(StructurePiece *startPiece, list<StructurePiece *> *pieces, Random *random);
        static CastleCorridorTBalconyPiece *createPiece(list<StructurePiece *> *pieces, Random *random, int footX, int footY, int footZ, int direction, int genDepth);
        virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);
    };
};
