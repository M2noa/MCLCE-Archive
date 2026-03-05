/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "CommonTypes.h"

#include "client/renderer/texture/TextureUVCoordinateSet.h"
// #include "Renderer/Mesh.h"
#include "world/phys/AABB.h"
// #include "Renderer/MaterialPtr.h"
#include "Core/Math/Color.h"
#include "client/renderer/block/BlockTessellatorCache.h"
#include "world/phys/Vec2.h"
// #include "Renderer/TexturePtr.h"
#include "world/level/block/BlockRenderLayer.h"

class Block;
class FenceBlock;
class FenceGateBlock;
class ThinFenceBlock;
class StairBlock;
class BlockSource;
class Material;
class Tessellator;
class WallBlock;
class BaseRailBlock;
class CocoaBlock;
class DoublePlantBlock;
class EndPortalFrameBlock;
class FlowerPotBlock;
class AnvilBlock;
class ObserverBlock;
class BrewingStandBlock;
class BlockOccluder;
class StemBlock;
class LiquidBlock;
class ChorusPlantBlock;
class BlockGraphics;
enum class BlockShape : int;
enum class AnvilPart;

//this structure represent the key of a <Block in world, Mesh> relation, eg. for Moving Blocks & other blocks that are standalone but are shaded.
struct BlockTessellatorBlockInWorld {
	FullBlock block;
	BlockRenderLayer layer;
	BlockPos pos;
	const void* owner;

	bool operator==(const BlockTessellatorBlockInWorld& rhs) const {
		return block == rhs.block && layer == rhs.layer && pos == rhs.pos && owner == rhs.owner;
	}
};
 
namespace std {
	///hash specialization for unordered_maps
	template<>
	struct hash<BlockTessellatorBlockInWorld> {
		// hash functor for vector
		size_t operator()(const BlockTessellatorBlockInWorld& key) const {
			size_t seed = 0;
			seed = Math::hash_accumulate(seed, key.block.toInt());
			seed = Math::hash_accumulate(seed, enum_cast(key.layer));
			seed = Math::hash_accumulate(seed, key.pos.x);
			seed = Math::hash_accumulate(seed, key.pos.y);
			seed = Math::hash_accumulate(seed, key.pos.z);
			seed = Math::hash_accumulate(seed, key.owner);
			return seed;
		}
	};
}


class BlockTessellator {
public:
	static const float TEXTURE_OFFSET;

	BlockTessellator(Tessellator& t, BlockSource* level = nullptr);

	void setRegion(BlockSource& region);
	BlockSource& getRegion() const;

	void setRenderLayer(int layer);
	void setSmoothLightingEnabled(bool enabled) {
		mSmoothLighting = enabled;
	}

	float getWaterHeight(const BlockPos& p, const Material& m);

	int blend(int a, int b, int c, int def);

	const bool isTranslucent(const Block& block);
	bool isSolidRender(const Block& block);

	void setForceOpaque(bool forceOpaque);

// 	mce::Mesh& getMeshForBlock( const FullBlock& block );
// 	mce::Mesh& getMeshForBlockInWorld( const FullBlock& block, BlockPos& blockPos, BlockRenderLayer layer, const void* owner);

	int getLightEmission(const Block& block);

	void tessellateInWorld(const Block& block, const BlockPos& p, DataID data, const TextureUVCoordinateSet& fixedTexture);
	bool tessellateInWorld(const Block& block, const BlockPos& p, DataID data, bool renderBothSides = false);

	BrightnessPair getLightColor(const BlockPos& belowPos);

	void appendTessellatedBlock(const FullBlock& fullBlock);
// 	void renderGuiBlock(const FullBlock& fullBlock, const mce::TexturePtr& texture, float lightMultiplyer = 1.0f, float alphaMultiplyer = 1.0f);

	static bool canRender(BlockShape renderShape);

	void clearBlockCache();
	void resetCache(const BlockPos& pos, BlockSource& mRegion);

	Tessellator& getTessellator();

	bool tessellateStructureVoidInWorld(const Block& block, const BlockPos& pos, int data, bool render = false);

	void setUnrealActor(AActor* unrealActor);

protected:
	/*
		!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		PLEASE; dont check using region and blockpos what the data is, the tesselator might render using other 'data' 
		!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	*/
	bool tessellateBlockInWorld(const Block& block, const BlockPos& pos, int data);
	bool tessellateBlockInWorldFlat(const Block& block, const BlockPos& pos, int data, const Color& base, BlockOccluder* occluder);
	bool tessellateBlockInWorldWithAmbienceOcclusion(const Block& block, BlockPos p, int data, const Color& base, BlockOccluder* occluder);

	bool tessellateExtraDataInWorld(const Block& block, const BlockPos& p);
	bool tessellateCrossInWorld(const Block& block, const BlockPos& p, DataID data, bool renderBothSides);
	bool tessellateCrossPolyInWorld(const Block& block, const BlockPos& p, DataID data, bool renderBothSides);
	void tessellateCrossTexture(const Block& block, DataID data, const Vec3& pos, bool renderBothSides, float scale = 1.0f);
	void tessellateCrossTexture(const TextureUVCoordinateSet& tex, const Vec3& pos, bool renderBothSides, const Block& block, float scale = 1.0f);
	void tessellateCrossPolyTexture(const TextureUVCoordinateSet& tex, const Vec3& pos, bool renderBothSides, const Block& block);
	bool tessellateLiquidInWorld(const LiquidBlock& block, const BlockPos& p, DataID data);
	bool tessellateWallInWorld(const WallBlock& block, const BlockPos& p, int data);
	bool tessellateRailInWorld(const BaseRailBlock& block, const BlockPos& p, int data);

	bool tessellateTopSnowInWorld(const Block& block, const BlockPos& p, DataID data);
	bool tessellateTorchInWorld(const Block& block, const BlockPos& p, DataID data);
	bool tessellateFireInWorld(const Block& block, BlockPos p);
	bool tessellateLadderInWorld(const Block& block, const BlockPos& p, DataID data);
	bool tessellateCactusInWorld(const Block& block, const BlockPos& p, int data);
	bool tessellateDoublePlantInWorld(const DoublePlantBlock& block, const BlockPos& p, DataID data, bool preferPoly, bool renderBothSides);
	bool tessellateStemInWorld(const StemBlock& block, const BlockPos& p);
	bool tessellateStairsInWorld(const StairBlock& block, const BlockPos& pos, int data);
	bool tessellateDoorInWorld(const Block& block, const BlockPos& p, int data);
	bool tessellateFenceInWorld(const FenceBlock& block, const BlockPos& p, int data);
	bool tessellateThinFenceInWorld(const ThinFenceBlock& block, const BlockPos& p, DataID data);
	bool tessellateFenceGateInWorld(const FenceGateBlock& block, const BlockPos& p, DataID data);
	bool tessellateBedInWorld(const Block& block, const BlockPos& p, DataID data);
	bool tessellateRowInWorld(const Block& block, const BlockPos& p, DataID data);
	bool tessellateVineInWorld(const Block& block, const BlockPos& p, DataID data);
	bool tessellateCocoaInWorld(const CocoaBlock& block, const BlockPos& p, DataID data);
	bool tessellateEndPortalFrameInWorld(const EndPortalFrameBlock& block, const BlockPos& pos, DataID data);
	bool tessellateTreeInWorld(const Block& block, const BlockPos& p, DataID data);
	void tessellateTorch(const Block& block, const Vec3& pos, float xxa, float zza);
	void tessellateStemTexture(const Block& block, DataID data, float h, float x, float y, float z);
	void tessellateStemDirTexture(const StemBlock& block, DataID data, int dir, float h, const Vec3& pos);
	void tessellateRowTexture(const Block& block, DataID data, float x, float y, float z);
	bool tessellateFlowerPotInWorld(const FlowerPotBlock& block, const BlockPos& p, int data);
	bool tessellateAnvilInWorld(const AnvilBlock& block, const BlockPos& p);
	bool tessellateAnvilInWorld(const AnvilBlock& block, const BlockPos& p, int data);
	bool tessellateAnvilInWorld(const AnvilBlock& block, const BlockPos& p, int data, bool render);
	bool tessellateBrewingStandInWorld(const BrewingStandBlock& block, const BlockPos& pos, int data);
	bool tessellateLilypadInWorld(const Block& block, const BlockPos& p);
	float tessellateAnvilPiece(const AnvilBlock& block, const BlockPos& p, AnvilPart part, float bottom, float width, float height, float length, bool rotate, bool render, int data, int* faces, int num_faces);
	bool tessellateLeverInWorld(const Block& block, const BlockPos& pos, int data);
	bool tessellateDustInWorld(const Block& b, const BlockPos &p, DataID data);
	bool tessellateTripwireHookInWorld(const Block& b, const BlockPos& p, int data);
	bool tessellateTripwireInWorld(const Block& b, const BlockPos& p, int data);
	bool tessellateCauldronInWorld(const Block& b, const BlockPos& p, DataID data);
	bool tessellateRepeaterInWorld(const Block& block, const BlockPos& p, int data);
	bool tessellateComparatorInWorld(const Block& block, const BlockPos& p, int data);
	bool tessellateDiodeInWorld(const Block& block, const BlockPos& p, int dir);
	bool tessellateHopperInWorld(const Block& block, const BlockPos& pos, int data);
	bool tessellateHopperInWorld(const Block& block, const BlockPos& pos, int data, bool render);
	bool tessellateSlimeBlockInWorld(const Block& block, const BlockPos& pos, int data);
	bool tessellatePistonInWorld(const Block& block, const BlockPos& pos, int data);
	bool tessellateBeaconInWorld(const Block& block, const BlockPos& pos, int data);
	void tessellateBeacon(const Block& block, float lightMultiplier, const Vec3& vOffset = Vec3::ZERO);
	bool tessellateDragonEgg(const Block& block, const BlockPos& pos, int data, bool render);
	bool tessellateChorusFlowerInWorld(const Block& block, const BlockPos& pos, int data, bool render = false);
	bool tessellateChorusPlantInWorld(const ChorusPlantBlock& block, const BlockPos& pos, int data);
	bool tessellateEndPortalInWorld(const Block& block, const BlockPos& pos, int data);
	bool tessellateEndGatewayInWorld(const Block& block, const BlockPos& pos, int data);
	bool tessellateEndRodInWorld(const Block& b, const BlockPos& p, DataID data);
	void tessellateEndRodUp(const BlockPos& p, const TextureUVCoordinateSet& tex);
	void tessellateEndRodDown(const BlockPos& p, const TextureUVCoordinateSet& tex);
	void tessellateEndRodNorth(const BlockPos& p, const TextureUVCoordinateSet& tex);
	void tessellateEndRodSouth(const BlockPos& p, const TextureUVCoordinateSet& tex);
	void tessellateEndRodWest(const BlockPos& p, const TextureUVCoordinateSet& tex);
	void tessellateEndRodEast(const BlockPos& p, const TextureUVCoordinateSet& tex);
	void tessellateEndRodAppended(const Block& block, const BlockGraphics& blockGraphics);
	void tessellateEndRodGui(const Block& block, const BlockGraphics& blockGraphics);

	void renderFaceDown(const Block& block, const Vec3& p, const TextureUVCoordinateSet& tex);
	void renderFaceUp(const Block& block, const Vec3& p, const TextureUVCoordinateSet& tex);
	void renderNorth(const Block& block, const Vec3& p, const TextureUVCoordinateSet& tex);
	void renderSouth(const Block& block, const Vec3& p, const TextureUVCoordinateSet& tex);
	void renderWest(const Block& block, const Vec3& p, const TextureUVCoordinateSet& tex);
	void renderEast(const Block& block, const Vec3& p, const TextureUVCoordinateSet& tex);
	void renderAll(const Block& block, const Vec3& p, const TextureUVCoordinateSet& tex);

	void renderFaceWithUVs(const Block& block, const Vec3& p, const TextureUVCoordinateSet& tex, const float u0, const float v0, const float u1, const float v1, const int face);
	void renderVerticesAndUVs(const TextureUVCoordinateSet& tex, const Vec3& a, const Vec3& b, Vec2& first, Vec2& second, Vec2& third, Vec2& fourth, const int face);
	void offsetUVs(const TextureUVCoordinateSet& tex, Vec2* first, Vec2* second = nullptr, Vec2* third = nullptr, Vec2* fourth = nullptr);

private:
	Color _getBlockColor(const BlockPos& p, const Block& block);
	Color _getBlockColor(const BlockPos& p, const Block& block, DataID data);
	Vec3 _preparePolyCross(const BlockPos& p, const Block& block);
	void _randomizeFaceDirection(const Block& block, FacingID face, const BlockPos& p);

	// Helper function to render all 6 directions of a block
	void _renderAllFaces(const Block& block, const BlockPos& pos, int data);

	float _occlusion(const BlockPos& pos) const;
	float _findDepth(const BlockPos& p, float baseDepth);
	BrightnessPair _getLightColorForWater(const BlockPos& pos, BrightnessPair insideBlock);

	const TextureUVCoordinateSet& _getTexture(const Block& block, FacingID face, int data) const;
	DataID getData(const BlockPos& p) const;

	void _tex1(const Vec2& uv);

	static const int FLIP_NONE = 0, FLIP_CW = 1, FLIP_CCW = 2, FLIP_180 = 3;

	bool mRenderingExtra = false;
	bool mUseNormals = false;

	BlockSource* mRegion;

	TextureUVCoordinateSet mFixedTexture;
	bool mUseFixedTexture;
	bool mUseOccluder = true;
	bool _tmpUseRegion = true;	// TODO: remove when BlockModels!
	bool mXFlipTexture;
	int mRenderingLayer = 0;
	bool mRenderingGUI = false;
	DataID mRenderingData = 0;

	bool mApplyAmbientOcclusion;
	bool mSmoothLighting = true;
	Color mAoColors[8];
	// Used for ambient occlusion blending
	BrightnessPair mTc[4];

	int mFlipFace[6];

	// Cache for directionBlocks for ambient occlusion (needs to be reset between use!)
	const Block* mAmbientOcclusionCacheBlocks[5 * 5 * 5];	//	@note: Currently not used, should this be removed?

// 	mce::MaterialPtr mItemMat;

	Tessellator& mTessellator;
	// The current render shape
	AABB mCurrentShape;
	
// 	std::unordered_map<int, mce::Mesh> mBlockMeshes;
// 	std::unordered_map<BlockTessellatorBlockInWorld, mce::Mesh> mInWorldBlockMeshes;

	BlockTessellatorCache mBlockCache;

	bool mForceOpaque = false;

	void _setShapeAndTessellate(const Vec3& min, const Vec3& max, const Block& block, const BlockPos& pos, const int data);

	AActor* mUnrealActor = nullptr;

public:
	void clear();
};
