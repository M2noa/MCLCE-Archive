#pragma once

#include "world/level/levelgen/v1/LargeFeature.h"
#include "CommonTypes.h"
#include "world/level/ChunkPos.h"
#include "StructureStart.h"
#include "StructurePiece.h"
#include "world/level/levelgen/structure/StructureTemplateHelpers.h"
#include "world/level/levelgen/structure/StructureTemplate.h"

class StructureTemplate;

class ScatteredFeaturePiece : public StructurePiece {
	protected:
		int mWidth, mHeight, mDepth;
		int mHeightPosition = -1;

		ScatteredFeaturePiece(int west, int floor, int north, int width, int height, int depth);

		virtual void addAdditionalSaveData(CompoundTag& tag) const override;
		virtual void readAdditionalSaveData(const CompoundTag& tag) override;

		bool updateAverageGroundHeight(BlockSource *region, const BoundingBox& chunkBB, int offset);
};

class DesertPyramidPiece : public ScatteredFeaturePiece {
	private: 
		bool mHasPlacedChest[4];

	public:
		DesertPyramidPiece(int west, int north);
		virtual bool postProcess(BlockSource* region, Random& random, const BoundingBox& chunkBB) override;
		StructurePieceType getType() const override {
			return StructurePieceType::DesertPyramid;
		}


	protected:		
		virtual void addAdditionalSaveData(CompoundTag& tag) const override;
		virtual void readAdditionalSaveData(const CompoundTag& tag) override;
};

class MossStoneSelector : public BlockSelector {
public:
	void next(Random& random, int worldX, int worldY, int worldZ, bool isEdge) override;
};

class JunglePyramidPiece : public ScatteredFeaturePiece {
private:
	bool mHasPlacedMainChest;
	bool mHasPlacedHiddenChest;
	bool mHasPlacedTrap[2];

public:
	JunglePyramidPiece(int west, int north);
	virtual bool postProcess(BlockSource* region, Random& random, const BoundingBox& chunkBB) override;
	StructurePieceType getType() const override {
		return StructurePieceType::JunglePyramid;
	}


	static MossStoneSelector stoneSelector;

protected:
	virtual void addAdditionalSaveData(CompoundTag& tag) const override;
	virtual void readAdditionalSaveData(const CompoundTag& tag) override;
};

class SwamplandHut : public ScatteredFeaturePiece {
private:
	bool mSpawnedWitch = false;

public:
	SwamplandHut(int west, int north);
	StructurePieceType getType() const override {
		return StructurePieceType::WitchHut;
	}


protected:
	virtual void addAdditionalSaveData(CompoundTag& tag) const override;
	virtual void readAdditionalSaveData(const CompoundTag& tag) override;
	void placeCauldron(BlockSource* region, Random& random, int x, int y, int z, const BoundingBox& chunkBB);

public:
	virtual bool postProcess(BlockSource* region, Random& random, const BoundingBox& chunkBB) override;
	virtual void postProcessMobsAt(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;
};

class Igloo : public ScatteredFeaturePiece {
public:
	Igloo(int west, int north);
	StructurePieceType getType() const override {
		return StructurePieceType::Igloo;
	}

	virtual bool postProcess(BlockSource* region, Random& random, const BoundingBox& chunkBB) override;
	virtual void postProcessMobsAt(BlockSource* region, Random& random, const BoundingBox& chunkBB) override;

private:
	static const std::string STRUCTURE_LOCATION_IGLOO_TRAPDOOR;
	static const std::string STRUCTURE_LOCATION_IGLOO_NO_TRAPDOOR;
	static const std::string STRUCTURE_LOCATION_LADDER;
	static const std::string STRUCTURE_LOCATION_LABORATORY;

	StructureSettings mLabSettings;
	BlockPos mLabPos;
};

