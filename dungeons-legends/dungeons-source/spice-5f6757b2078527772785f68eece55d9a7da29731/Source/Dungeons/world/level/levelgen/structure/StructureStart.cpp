#include "Dungeons.h"

#include "world/level/levelgen/structure/StructureStart.h"
#include "world/level/levelgen/structure/StructureManager.h"
#include "world/level/levelgen/structure/StructurePiece.h"
#include "world/level/levelgen/structure/StructurePieceFactory.h"

#include "nbt/CompoundTag.h"
#include "nbt/IntArrayTag.h"
#include "world/level/Level.h"
#include "nbt/ListTag.h"
#include "world/level/LevelConstants.h"

bool StructureStart::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	PieceList::iterator it = pieces.begin();

	bool postProcessed = false;

	while (it != pieces.end()) {
		Unique<StructurePiece>& next = (*it);
		if (next->getBoundingBox().intersects(chunkBB)) {
			if (!next->postProcess(level, random, chunkBB)) {
				it = pieces.erase(it);
				continue;
			}
			else {
				postProcessed = true;
			}
		}
		++it;
	}

	return postProcessed;
}

void StructureStart::postProcessMobsAt(BlockSource* blockSource, Random& random, const BoundingBox& chunkBB) {
	PieceList::iterator it = pieces.begin();

	while (it != pieces.end()) {
		Unique<StructurePiece>& next = (*it);
		next->postProcessMobsAt(blockSource, random, chunkBB);
		++it;
	}
}

Unique<CompoundTag> StructureStart::createTag(int chunkX, int chunkZ) {
	auto tag = make_unique<CompoundTag>();

	// TODO
	//tag.putString("id", StructureFeatureIO::getEncodeId(this));
	tag->putInt("ID", static_cast<int>(getType()));
	tag->putInt("ChunkX", chunkX);
	tag->putInt("ChunkZ", chunkZ);
	tag->put("BB", boundingBox.createTag());

	auto childrenTags = make_unique<ListTag>();

	for (unsigned int i = 0; i < pieces.size(); i++) {
		Unique<CompoundTag> childPiece = pieces[i]->createTag();
		if (childPiece) {
			childrenTags->add(std::move(childPiece));
		}
	}
	tag->put("Children", std::move(childrenTags));

	addAdditionalSaveData(*tag.get());

	return tag;
}

void StructureStart::load(const CompoundTag& tag, Level& level) {
	chunkX = tag.getInt("ChunkX");
	chunkZ = tag.getInt("ChunkZ");
	if (tag.contains("BB")) {
		boundingBox = BoundingBox((int*)tag.getIntArray("BB").data());
	}

	const ListTag* children = tag.getList("Children");
	if (children != nullptr) {
		for (int i = 0; i < children->size(); ++i) {
			const CompoundTag* pieceTag = children->getCompound(i);
			if (pieceTag != nullptr) {
				int id = pieceTag->getInt("ID");
				Unique<StructurePiece> newPiece = StructurePieceFactory::createStructurePiece(static_cast<StructurePieceType>(id), level.getStructureManager());
				if (newPiece) {
					newPiece->loadFromTag(*pieceTag);
					pieces.emplace_back(std::move(newPiece));
				}
			}
		}
	}

	readAdditionalSaveData(tag);
}

void StructureStart::addAdditionalSaveData(CompoundTag& tag) {

}

void StructureStart::readAdditionalSaveData(const CompoundTag& tag) {

}

void StructureStart::calculateBoundingBox() {
	boundingBox = BoundingBox::getUnknownBox();

	for (auto& piece : pieces) {
		boundingBox.expand(piece->getBoundingBox());
	}
}

void StructureStart::moveToLevel(Random& random, int maxY) {
	// set lowest possible position (at bedrock)
	int y1Pos = boundingBox.getYSpan() + 1;
	// move up randomly within the available span
	if (y1Pos < maxY) {
		y1Pos += random.nextInt(maxY - y1Pos);
	}

	// move all bounding boxes
	int dy = y1Pos - boundingBox.y1;
	boundingBox.move(0, dy, 0);

	for (unsigned int i = 0; i < pieces.size(); i++) {
		pieces[i]->moveBoundingBox(0, dy, 0);
	}
}

void StructureStart::moveInsideHeights(Random* random, int lowestAllowed, int highestAllowed) {
	int heightSpan = highestAllowed - lowestAllowed + 1 - boundingBox.getYSpan();
	int y0Pos = 1;

	if (heightSpan > 1) {
		y0Pos = lowestAllowed + random->nextInt(heightSpan);
	}
	else {
		y0Pos = lowestAllowed;
	}

	// move all bounding boxes
	int dy = y0Pos - boundingBox.y0;
	boundingBox.move(0, dy, 0);

	for (unsigned int i = 0; i < pieces.size(); i++) {
		pieces[i]->moveBoundingBox(0, dy, 0);
	}

}
