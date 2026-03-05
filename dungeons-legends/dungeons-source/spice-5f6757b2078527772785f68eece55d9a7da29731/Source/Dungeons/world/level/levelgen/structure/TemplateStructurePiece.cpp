#include "Dungeons.h"

#include "world/level/levelgen/structure/TemplateStructurePiece.h"

#include "nbt/CompoundTag.h"
#include "world/level/BlockSource.h"
#include "world/level/levelgen/structure/StructureTemplate.h"

TemplateStructurePiece::TemplateStructurePiece(int depth)
	: StructurePiece(depth)
	, mTemplate(nullptr)
	, mSettings()
	, mTemplatePosition() {

	mSettings.setIgnoreEntities(true);
	mSettings.setIgnoreBlock(Block::mAir);
}

void TemplateStructurePiece::_setup(StructureTemplate& structTemplate, const StructureSettings& settings, const BlockPos& templatePosition) {

	mTemplate = &structTemplate;
	mSettings = settings;
	mTemplatePosition = templatePosition;

	orientation = Direction::NORTH;
	_setBoundingBoxFromTemplate();
}

void TemplateStructurePiece::addAdditionalSaveData(CompoundTag& tag) const {
	tag.put("TemplatePosition", ListTagIntAdder(mTemplatePosition.x)(mTemplatePosition.y)(mTemplatePosition.z).done());
}

void TemplateStructurePiece::readAdditionalSaveData(const CompoundTag& tag) {
	auto pos = tag.getList("TemplatePosition");
	mTemplatePosition.x = pos->getInt(0);
	mTemplatePosition.y = pos->getInt(1);
	mTemplatePosition.z = pos->getInt(2);
}

bool TemplateStructurePiece::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	if (mTemplate == nullptr) {
		return false;
	}

	mSettings.setBoundingBox(chunkBB);
	mTemplate->placeInWorld(*level, mTemplatePosition, mSettings);

	auto markers = mTemplate->getMarkers(mTemplatePosition, mSettings);
	for (auto&& marker : markers) {
		_handleDataMarker(marker.second, marker.first, *level, random, chunkBB);
	}

	return true;
}

void TemplateStructurePiece::moveBoundingBox(int dx, int dy, int dz) {
	StructurePiece::moveBoundingBox(dx, dy, dz);
	mTemplatePosition = mTemplatePosition.offset(dx, dy, dz);
}

void TemplateStructurePiece::_setBoundingBoxFromTemplate() {
	Rotation rotation = mSettings.getRotation();
	BlockPos size = mTemplate->getSize(rotation);
	boundingBox = BoundingBox(0, 0, 0, size.x, size.y - 1, size.z);

	switch (rotation) {
	case Rotation::ROTATE_90:
		boundingBox.move(-size.x, 0, 0);
		break;
	case Rotation::ROTATE_270:
		boundingBox.move(0, 0, -size.z);
		break;
	case Rotation::ROTATE_180:
		boundingBox.move(-size.x, 0, -size.z);
		break;
	default:
		break;
	}

	boundingBox.move(mTemplatePosition.x, mTemplatePosition.y, mTemplatePosition.z);
}
