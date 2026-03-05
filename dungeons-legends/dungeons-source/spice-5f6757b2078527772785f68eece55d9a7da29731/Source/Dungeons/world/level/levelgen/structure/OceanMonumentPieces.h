/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once
#include "world/level/levelgen/structure/StructurePiece.h"
#include "world/level/block/Block.h"

class RoomDefinition : public std::enable_shared_from_this<RoomDefinition> {
public:
	RoomDefinition();
	RoomDefinition(int roomIndex);
	void setConnection(const FacingID& direction, Shared<RoomDefinition> definition);
	void updateOpenings();
	bool findSource(int scanIndex);
	bool isSpecial();
	int countOpenings();

	int mIndex;
	std::vector<Shared<RoomDefinition>> mConnections;
	std::vector<bool> mHasOpening;
	bool mClaimed;
	bool mIsSource;
	int mScanIndex; // used for path:ing
};

class OceanMonumentPiece : public StructurePiece {
public:
	OceanMonumentPiece();
	OceanMonumentPiece(int genDepth);
	OceanMonumentPiece(int& orientation, BoundingBox& boundingBox);
	virtual bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override = 0;
	virtual void postProcessMobsAt(BlockSource* region, Random& random, const BoundingBox& chunkBB) override;
	virtual void addAdditionalSaveData(CompoundTag& tag) const override;
	virtual void readAdditionalSaveData(const CompoundTag& tag) override;
	virtual int getWorldZ(int x, int z) override;
	virtual int getWorldX(int x, int z) override;

	static int getGridSize() {
		return OceanMonumentPiece::mGridSize;
	}

protected:
	OceanMonumentPiece(int genDepth, int& orientation, Shared<RoomDefinition>& roomDefinition, int roomWidth, int roomHeight, int roomDepth);

	const static int getRoomIndex(int roomX, int roomY, int roomZ) {
		return (roomY * OceanMonumentPiece::mGridFloorCount) + (roomZ * OceanMonumentPiece::mGridWidth) + roomX;
	}

	// Returns the coordinate inside the monument's overall bounding box for the west/south corner of the room
	// (remember: when constructing structures, z coordinates grow in the north direction, so south starts at 0).
	const static int getRoomX(int roomX) {
		return 9 + roomX * OceanMonumentPiece::mGridroomWidth;
	}
	const static int getRoomY(int roomY) {
		return roomY * OceanMonumentPiece::mGridroomHeight;
	}
	static int getRoomZ(int roomZ) {
		return 22 + roomZ * OceanMonumentPiece::mGridroomDepth;
	}

	void generateWaterBox(BlockSource* level, const BoundingBox& chunkBB, int x0, int y0, int z0, int x1, int y1, int z1, bool skipAir);
	void generateDefaultFloor(BlockSource* level, const BoundingBox& chunkBB, int xOff, int zOff, bool downOpening);
	void generateBoxOnFillOnly(BlockSource* level, const BoundingBox& chunkBB, int x0, int y0, int z0, int x1, int y1, int z1, FullBlock targetBlock);
	bool chunkIntersects(const BoundingBox& chunkBB, int x0, int z0, int x1, int z1);
	
	void spawnElder(BlockSource* level, const BoundingBox& chunkBB, int x, int y, int z);


	bool mDoFill = true;

	const static int mGridroomWidth = 8;
	const static int mGridroomDepth = 8;
	const static int mGridroomHeight = 4; // floor is other rooms' ceiling (1 block floor + 3 blocks wall)
	const static int mGridWidth = 5;
	const static int mGridDepth = 5;
	const static int mGridHeight = 3;
	const static int mGridFloorCount = mGridWidth * mGridDepth;
	const static int mGridSize = mGridFloorCount * mGridHeight;
	const static int mLeftWingIndex;
	const static int mRightWingIndex;
	const static int mPenthouseIndex;
	static int mGridroomSourceIndex; // entry room
	static int mGridroomTopConnectIndex; // room with connection to roof piece
	static int mGridroomLeftWingConnectIndex; // room with connection to left wing piece
	static int mGridroomRightWingConnectIndex; // room with connection to right wing piece

	Shared<RoomDefinition> mRoomDefinition;
};

class MonumentBuilding : public OceanMonumentPiece {
public:
	MonumentBuilding();
	MonumentBuilding(Random& random, int west, int north, int& dir);

	StructurePieceType getType() const override {
		return StructurePieceType::OceanMonumentBuilding;
	}

	virtual bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;
	virtual void postProcessMobsAt(BlockSource* region, Random& random, const BoundingBox& chunkBB) override;
	const static int getBiomeRange() {
		return mBiomeRangeCheck;
	}

private:
	std::vector<Shared<RoomDefinition>> generateRoomGraph(Random& random);
	void generateWing(bool isFlipped, int xoff, BlockSource* level, Random& random, const BoundingBox& chunkBB);
	void generateEntranceArchs(BlockSource* level, Random& random, const BoundingBox& chunkBB);
	void generateEntranceWall(BlockSource* level, Random& random, const BoundingBox& chunkBB);
	void generateRoofPiece(BlockSource* level, Random& random, const BoundingBox& chunkBB);
	void generateLowerWall(BlockSource* level, Random& random, const BoundingBox& chunkBB);
	void generateMiddleWall(BlockSource* level, Random& random, const BoundingBox& chunkBB);
	void generateUpperWall(BlockSource* level, Random& random, const BoundingBox& chunkBB);

	std::vector<Unique<OceanMonumentPiece>> mChildPieces;
	std::vector<Shared<RoomDefinition>> mRoomGrid;
	Shared<RoomDefinition> mSourceRoom;
	Shared<RoomDefinition> mCoreRoom;
	const static int mWidth = 58; // 636 - 578;
	const static int mHeight = 22; //85 - 63;
	const static int mDepth = 58; // 532 - 474;
	const static int mBiomeRangeCheck = mWidth / 2;
	const static int mTopPosition = 61;
};


class OceanMonumentEntryRoom : public OceanMonumentPiece {
public:
	OceanMonumentEntryRoom();
	OceanMonumentEntryRoom(int& orientation, Shared<RoomDefinition>& definition);
	StructurePieceType getType() const override {
		return StructurePieceType::OceanMonumentEntryRoom;
	}

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;
};

class OceanMonumentSimpleRoom : public OceanMonumentPiece {
public:
	OceanMonumentSimpleRoom();
	OceanMonumentSimpleRoom(int& orientation, Shared<RoomDefinition>& definition, Random& random);
	StructurePieceType getType() const override {
		return StructurePieceType::OceanMonumentSimpleRoom;
	}

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;
private:
	int mMainDesign;
};

class OceanMonumentSimpleTopRoom : public OceanMonumentPiece {
public:
	OceanMonumentSimpleTopRoom();
	OceanMonumentSimpleTopRoom(int& orientation, Shared<RoomDefinition>& definition, Random& random);
	StructurePieceType getType() const override {
		return StructurePieceType::OceanMonumentSimpleTopRoom;
	}

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;
};

class OceanMonumentDoubleYRoom : public OceanMonumentPiece {
public:
	OceanMonumentDoubleYRoom();
	OceanMonumentDoubleYRoom(int& orientation, Shared<RoomDefinition>& definition, Random& random);
	StructurePieceType getType() const override {
		return StructurePieceType::OceanMonumentDoubleYRoom;
	}

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;
};

class OceanMonumentDoubleXRoom : public OceanMonumentPiece {
public:
	OceanMonumentDoubleXRoom();
	OceanMonumentDoubleXRoom(int& orientation, Shared<RoomDefinition>& definition, Random& random);
	StructurePieceType getType() const override {
		return StructurePieceType::OceanMonumentDoubleXRoom;
	}

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;
};


class OceanMonumentDoubleZRoom : public OceanMonumentPiece {
public:
	OceanMonumentDoubleZRoom();
	OceanMonumentDoubleZRoom(int& orientation, Shared<RoomDefinition>& definition, Random& random);
	StructurePieceType getType() const override {
		return StructurePieceType::OceanMonumentDoubleZRoom;
	}

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;
};

class OceanMonumentDoubleXYRoom : public OceanMonumentPiece {
public:
	OceanMonumentDoubleXYRoom();
	OceanMonumentDoubleXYRoom(int& orientation, Shared<RoomDefinition>& definition, Random& random);
	StructurePieceType getType() const override {
		return StructurePieceType::OceanMonumentDoubleXYRoom;
	}

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;
};

class OceanMonumentDoubleYZRoom : public OceanMonumentPiece {
public:
	OceanMonumentDoubleYZRoom();
	OceanMonumentDoubleYZRoom(int& orientation, Shared<RoomDefinition>& definition, Random& random);
	StructurePieceType getType() const override {
		return StructurePieceType::OceanMonumentDoubleYZRoom;
	}

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;
};

class OceanMonumentCoreRoom : public OceanMonumentPiece {
public:
	OceanMonumentCoreRoom();
	OceanMonumentCoreRoom(int& orientation, Shared<RoomDefinition>& definition, Random& random);
	StructurePieceType getType() const override {
		return StructurePieceType::OceanMonumentCoreRoom;
	}

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;
};

class OceanMonumentWingRoom : public OceanMonumentPiece {
public:
	OceanMonumentWingRoom();
	OceanMonumentWingRoom(int& orientation, BoundingBox& boundingBox, int random, bool isRightWing);
	StructurePieceType getType() const override {
		return StructurePieceType::OceanMonumentWingRoom;
	}

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;
	void postProcessMobsAt(BlockSource* region, Random& random, const BoundingBox& chunkBB) override;
	virtual void addAdditionalSaveData(CompoundTag& tag) const override;
	virtual void readAdditionalSaveData(const CompoundTag& tag) override;
private:
	int mMainDesign;
	bool mSpawnedElders = false;
	bool mIsRightWing = false;
};

class OceanMonumentPenthouse : public OceanMonumentPiece {
public:
	OceanMonumentPenthouse();
	OceanMonumentPenthouse(int& orientation, BoundingBox& boundingBox);
	StructurePieceType getType() const override {
		return StructurePieceType::OceanMonumentPenthouse;
	}

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;
	void postProcessMobsAt(BlockSource* region, Random& random, const BoundingBox& chunkBB) override;
	virtual void addAdditionalSaveData(CompoundTag& tag) const override;
	virtual void readAdditionalSaveData(const CompoundTag& tag) override;
private:
	bool mSpawnedElders = false;
};

class MonumentRoomFitter {
public:
	virtual ~MonumentRoomFitter() {
	}
	virtual bool fits(Shared<RoomDefinition>& definition) = 0;
	virtual Unique<OceanMonumentPiece> create(int& orientation, Shared<RoomDefinition>& definition, Random& random) = 0;
};

class FitSimpleRoom : public MonumentRoomFitter {
public:
	bool fits(Shared<RoomDefinition>& definition) override;
	Unique<OceanMonumentPiece> create(int& orientation, Shared<RoomDefinition>& definition, Random& random) override;
};

class FitSimpleTopRoom : public MonumentRoomFitter {
public:
	bool fits(Shared<RoomDefinition>& definition) override;
	Unique<OceanMonumentPiece> create(int& orientation, Shared<RoomDefinition>& definition, Random& random) override;
};

class FitDoubleYRoom : public MonumentRoomFitter {
public:
	bool fits(Shared<RoomDefinition>& definition) override;
	Unique<OceanMonumentPiece> create(int& orientation, Shared<RoomDefinition>& definition, Random& random) override;
};

class FitDoubleXRoom : public MonumentRoomFitter {
public:
	bool fits(Shared<RoomDefinition>& definition) override;
	Unique<OceanMonumentPiece> create(int& orientation, Shared<RoomDefinition>& definition, Random& random) override;
};

class FitDoubleZRoom : public MonumentRoomFitter {
public:
	bool fits(Shared<RoomDefinition>& definition) override;
	Unique<OceanMonumentPiece> create(int& orientation, Shared<RoomDefinition>& definition, Random& random) override;
};

class FitDoubleXYRoom : public MonumentRoomFitter {
public:
	bool fits(Shared<RoomDefinition>& definition) override;
	Unique<OceanMonumentPiece> create(int& orientation, Shared<RoomDefinition>& definition, Random& random) override;
};

class FitDoubleYZRoom : public MonumentRoomFitter {
public:
	bool fits(Shared<RoomDefinition>& definition) override;
	Unique<OceanMonumentPiece> create(int& orientation, Shared<RoomDefinition>& definition, Random& random) override;
};
