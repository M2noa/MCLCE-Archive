/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/

#pragma once

#include <string>

#include "world/level/block/Block.h"
#include "world/level/levelgen/structure/StructureTemplateHelpers.h"

// Forward declarations
class Level;

class StructureBlockInfo;
class StructureEntityInfo;
class BlockPalette;
class StructureSettings;


// This class is for loading in custom structures straight outta da file
class StructureTemplate {
public:
	// Constants
	static const std::string SIZE_TAG;
	static const std::string AUTHOR_TAG;
	static const std::string VERSION_TAG;
	static const std::string PALETTE_TAG;

	static const std::string BLOCKS_TAG;
	static const std::string BLOCK_TAG_POS;
	static const std::string BLOCK_TAG_STATE;
	static const std::string BLOCK_TAG_NBT;

	static const std::string ENTITIES_TAG;
	static const std::string ENTITY_TAG_POS;
	static const std::string ENTITY_TAG_BLOCKPOS;
	static const std::string ENTITY_TAG_NBT;

	static const std::string MINECRAFT_PREFIX;
	static const int STRUCTURE_VERSION;
	static const int CHUNK_SIZE;

	static const StructureSettings defaultSettings;

	// Functions
	StructureTemplate();
	StructureTemplate(const StructureTemplate &rhs);
	StructureTemplate& operator=(const StructureTemplate& rhs);

	BlockPos calculateConnectedPosition(const StructureSettings& settings1, const BlockPos& connection1, const StructureSettings& settings2, const BlockPos& connection2) const;
	std::unordered_map<BlockPos, std::string> getMarkers(const BlockPos& position, StructureSettings& settings);

	CompoundTag &save(CompoundTag &tag);
	void load(CompoundTag &tag);

	void placeInWorld(BlockSource& region, const BlockPos& pos, StructureSettings& settings);
	void placeInWorldChunk(BlockSource& region, const BlockPos& position, StructureSettings& settings);
	void placeEntities(BlockSource& region, const BlockPos& position, Mirror mirror, Rotation rotation, const BoundingBox& boundingBox);

	// Takes blocks from the world and fills in the palette and info arrays
	void fillFromWorld(BlockSource& region, const BlockPos& pos, const BlockPos& size, bool includeEntities /*, Block ignoreBlock*/);
	void fillEntityList(BlockSource& region, const BlockPos& minCorner, BlockPos& maxCorner);

	Vec3 _calculateRelativePosition(Vec3 pos, const StructureSettings& settings) const;
	Vec3 _transform(Vec3 pos, Mirror mirror, Rotation rotation) const;
	BlockPos _calculateRelativePosition(BlockPos pos, const StructureSettings& settings) const;
	BlockPos _transform(BlockPos pos, Mirror mirror, Rotation rotation) const;

	void setAuthor(std::string author);

	BlockPos getSize(Rotation rotation = Rotation::NONE);


private:
	std::string mAuthor;
	BlockPos mSize;

	BlockPalette mPalette;

	std::vector<StructureBlockInfo> mBlockInfo;
	std::vector<StructureEntityInfo> mEntityInfo;

	//////////////////////////////////////////////////////////////////////////
	// Mapping member functions defined in StructureTemplateMapping.cpp
	//////////////////////////////////////////////////////////////////////////

	// Mapping that takes the Java saved property strings and gives it in a data friendly form
	int _mapToProperty(const std::string &propertyString, const std::string &valueString, const std::string &blockName);

	// Mapping during placement that will handle rotation if necessary
	DataID _mapToData(BlockID currID, StructureBlockInfo &blockInfo, const StructureSettings &settings);

	// Mapping for saving data values to the Java block states
	void _mapToString(CompoundTag &paletteTag, DataID dataVariant);

	// Mapping of blocks that are defined different in Java
	void _mapToBlock(std::string& blockName, int& data);

	// Maps Java tags to MCPE
	Unique<CompoundTag> _mapTag(Unique<CompoundTag> originalTag);
};
