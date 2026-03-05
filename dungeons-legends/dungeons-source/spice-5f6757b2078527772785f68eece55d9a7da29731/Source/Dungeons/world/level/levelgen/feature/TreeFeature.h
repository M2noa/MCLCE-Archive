/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

#include "world/level/levelgen/feature/Feature.h"

class Material;

class TreeFeature : public Feature {
public:

	TreeFeature(Entity* placer, int trunkType, int leafType, bool addJungleFeatures = false);
	virtual bool place(BlockSource& region, const BlockPos& pos, Random& random) const override;
	bool place(BlockSource& region, const BlockPos& pos, Random& random, int treeHeight) const;

protected:

	int _getTrunkType() const;
	int _getLeafType() const;

	void _placeLeaf(BlockSource& region, const BlockPos& pos) const;
	bool _placeTrunk(BlockSource& region, const BlockPos& pos, Random& random, int treeHeight) const;

	bool _prepareSpawn(BlockSource& region, const BlockPos& pos, int treeHeight) const;

	bool _isFree(BlockID block) const;
	bool _isFree(const Material& material) const;

private:

	void _placeFallenTrunk(BlockSource& region, const BlockPos& pos, Random& random, int treeHeight) const;
	void _addVine(BlockSource& region, const BlockPos& pos, int dir) const;
	void _addJungleFeatures(BlockSource& region, const BlockPos& vinePos, Random& random) const;
	void _addCocoaPlants(BlockSource& region, const BlockPos& pos, Random& random, int treeHeight) const;

	int mTrunkType;
	int mLeafType;
	bool mAddJungleFeatures;

};
