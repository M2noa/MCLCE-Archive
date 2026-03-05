#pragma once

#include "CommonTypes.h"
#include "world/level/levelgen/structure/StructurePieceType.h"

class StructureManager;
class StructurePiece;

class StructurePieceFactory {
public:
	static Unique<StructurePiece> createStructurePiece(StructurePieceType type, StructureManager& structureManager);
};
