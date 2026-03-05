/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "world/level/levelgen/structure/TemplateStructurePiece.h"

class StructureManager;

class EndCityPieces {
public:
	static void init();
	static void startHouseTower(StructureManager& structureManager, const BlockPos& origin, Rotation rotation, PieceList& pieces, Random& random);

public: ///////////// End City Piece /////////////

	class EndCityPiece : public TemplateStructurePiece {
	public:
		EndCityPiece(StructureManager& structureManager);
		EndCityPiece(StructureManager& structureManager, std::string templateName, const BlockPos& origin, Rotation rotation, bool overwrite);

		StructurePieceType getType() const override {
			return StructurePieceType::EndCityPiece;
		}

		virtual void addAdditionalSaveData(CompoundTag& tag) const override;
		virtual void readAdditionalSaveData(const CompoundTag& tag) override;

		void postProcessMobsAt(BlockSource* region, Random& random, const BoundingBox& chunkBB) override;

	protected:
		virtual void _handleDataMarker(const std::string& markerId, const BlockPos& position, BlockSource& region, Random& random, const BoundingBox& chunkBB) override;

	private:
		void _loadAndSetup(const BlockPos& position);

		StructureManager& mStructureManager;
		std::string mTemplateName;
		Rotation mRotation;
		bool mOverwrite;

		std::vector<BlockPos> mMobPositions;
	};

private: ///////////// Section Generators /////////////

	class SectionGenerator;
	typedef std::vector<Unique<SectionGenerator>> Generators;
	enum SectionType {
		SectionTower,
		SectionFatTower,
		SectionBridge,
		SectionHouse
	};

	class SectionGenerator {
	public:
		virtual bool generate(StructureManager& structureManager, const Generators& generators, int genDepth, EndCityPiece* parent, const BlockPos& offset, PieceList& pieces, Random& random) = 0;
		virtual ~SectionGenerator() {  }
	};

	class TowerGenerator : public SectionGenerator {
	public:
		virtual bool generate(StructureManager& structureManager, const Generators& generators, int genDepth, EndCityPiece* parent, const BlockPos& offset, PieceList& pieces, Random& random) override;
	};

	class FatTowerGenerator : public SectionGenerator {
	public:
		virtual bool generate(StructureManager& structureManager, const Generators& generators, int genDepth, EndCityPiece* parent, const BlockPos& offset, PieceList& pieces, Random& random) override;
	};

	class TowerBridgeGenerator : public SectionGenerator {
	public:
		TowerBridgeGenerator();
		virtual bool generate(StructureManager& structureManager, const Generators& generators, int genDepth, EndCityPiece* parent, const BlockPos& offset, PieceList& pieces, Random& random) override;

	private:
		bool mShipCreated;
	};

	class HouseTowerGenerator : public SectionGenerator {
	public:
		virtual bool generate(StructureManager& structureManager, const Generators& generators, int genDepth, EndCityPiece* parent, const BlockPos& offset, PieceList& pieces, Random& random) override;
	};

private: ///////////// Members /////////////

	static Unique<EndCityPiece> _createPiece(StructureManager& structureManager, EndCityPiece* parent, const BlockPos& offset, const std::string& templateName, Rotation rotation, bool overwrite);
	static EndCityPiece* _addHelper(PieceList& pieces, Unique<EndCityPiece> piece);
	static bool _recursiveChildren(StructureManager& structureManager, const Generators& generators, SectionType type, int newGenDepth, EndCityPiece* parent, const BlockPos& offset, PieceList& pieces, Random& random);

	static StructureSettings mOverwriteSettings;
	static StructureSettings mInsertSettings;

	const static int MAX_GEN_DEPTH = 8;
};
