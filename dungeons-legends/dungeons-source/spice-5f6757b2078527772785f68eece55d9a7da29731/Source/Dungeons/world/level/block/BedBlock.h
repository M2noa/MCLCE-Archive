/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class Pos;

class BedBlock : public Block {
public:
	static const DataID HEAD_PIECE_DATA;
	static const DataID OCCUPIED_DATA;
	static const int HEAD_DIRECTION_OFFSETS[4][2];

	static bool isHeadPiece(int data);
	static bool isOccupied(int data);
	static void setOccupied(BlockSource& level, const BlockPos& pos, bool occupied);
	static bool findStandUpPosition(BlockSource& level, const BlockPos& pos, int skipCount, BlockPos& position);

	BedBlock(const std::string& nameId, int id);

	bool use(Player& player, const BlockPos& pos) const override;

	bool isInteractiveBlock() const override;
	
	int getVariant(int data) const override;
	FacingID getMappedFace(FacingID face, int data) const override;

	bool playerWillDestroy(Player& player, const BlockPos& pos, int data) const override;
	void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;

	bool getSecondPart(BlockSource& region, const BlockPos& pos, BlockPos& out) const override;

protected:
	virtual bool canBeSilkTouched() const override;

protected:
};
