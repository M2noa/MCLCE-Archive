/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "world/level/levelgen/structure/StructurePiece.h"
#include "world/level/levelgen/structure/StructureTemplateHelpers.h"

class CompountTag;
class StructureTemplate;

class TemplateStructurePiece : public StructurePiece {

public:
	TemplateStructurePiece(int depth);

	virtual void addAdditionalSaveData(CompoundTag& tag) const override;
	virtual void readAdditionalSaveData(const CompoundTag& tag) override;

	virtual bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;
	virtual void moveBoundingBox(int dx, int dy, int dz) override;

	const StructureTemplate* getTemplate() const {
		return mTemplate;
	}

	const StructureSettings& getSettings() const {
		return mSettings;
	}

	const BlockPos& getTemplatePosition() const {
		return mTemplatePosition;
	}

protected:
	void _setup(StructureTemplate& structTemplate, const StructureSettings& settings, const BlockPos& templatePosition);
	virtual void _handleDataMarker(const std::string& markerId, const BlockPos& position, BlockSource& region, Random& random, const BoundingBox& chunkBB) = 0;

	StructureTemplate* mTemplate;
	StructureSettings mSettings;
	BlockPos mTemplatePosition;

private:
	void _setBoundingBoxFromTemplate();

};
