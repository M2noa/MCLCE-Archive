/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "client/renderer/block/BlockTessellator.h"
#include "client/renderer/renderer/Tessellator.h"
#include "client/renderer/block/BlockGraphics.h"
#include "world/level/BlockSource.h"
#include "world/level/block/Block.h"
#include "world/level/block/DoorBlock.h"
#include "world/level/block/LiquidBlock.h"
#include "world/level/block/FenceBlock.h"
#include "world/level/block/FenceGateBlock.h"
#include "world/level/block/ThinFenceBlock.h"
#include "world/level/block/BedBlock.h"
#include "world/level/block/StemBlock.h"
#include "world/level/block/StairBlock.h"
#include "world/level/block/TopSnowBlock.h"
#include "world/Direction.h"
#include "world/Facing.h"
#include "world/level/block/FireBlock.h"
#include "world/level/block/RotatedPillarBlock.h"
#include "world/level/block/WallBlock.h"
#include "world/level/block/BaseRailBlock.h"
#include "world/level/block/VineBlock.h"
#include "world/level/block/CocoaBlock.h"
#include "client/renderer/block/BlockTextureTessellator.h"
#include "world/level/block/DoublePlantBlock.h"
#include "world/level/block/EndPortalFrameBlock.h"
#include "world/level/block/FlowerPotBlock.h"
// #include "world/level/block/entity/StructureBlockEntity.h" 
#include "world/level/block/AnvilBlock.h"
#include "world/level/block/BrewingStandBlock.h"
// #include "world/level/block/entity/FlowerPotBlockEntity.h"
#include "world/level/block/RedStoneWireBlock.h"
#include "world/level/block/TripWireBlock.h"
#include "world/level/block/TripWireHookBlock.h"
#include "world/level/block/DiodeBlock.h"
#include "world/level/block/RepeaterBlock.h"
#include "world/level/block/ComparatorBlock.h"
// #include "world/level/block/entity/CauldronBlockEntity.h"
#include "world/level/block/CauldronBlock.h"
#include "world/level/block/HopperBlock.h"
#include "world/level/block/PistonBlock.h"
#include "world/level/block/ObserverBlock.h"
#include "world/level/block/ChorusPlantBlock.h"
#include "world/level/block/ChorusFlowerBlock.h"
#include "world/level/block/EndRodBlock.h"

#include "world/level/biome/Biome.h"
// #include "client/renderer/renderer/RenderMaterialGroup.h"
#include "BlockOccluder.h"
#include "client/renderer/block/AmbientOcclusionCalculator.h"

// Unreal
#include "Components/PointLightComponent.h"


Vec2 toUV(BrightnessPair coords) {
	return{ coords.block / 16.f, coords.sky / 16.f };
}

// Fix for Bug 29166: Incorrect texture mapping for Torch on iOS. We do not know why this affects only iOS.
#ifndef MCPE_PLATFORM_IOS
const float BlockTessellator::TEXTURE_OFFSET = 0.0001f;
#else
const float BlockTessellator::TEXTURE_OFFSET = 0.0f;
#endif

BlockTessellator::BlockTessellator(Tessellator& tessellator, BlockSource* level)
	: mRegion(level)
	, mUseFixedTexture(false)
	, mXFlipTexture(false)
	, mApplyAmbientOcclusion(false)
	, mTessellator(tessellator)
// 	, mItemMat(mce::RenderMaterialGroup::common, "ui_item")
	, mBlockCache() {
	memset(mFlipFace, 0, sizeof(mFlipFace));
	memset(mTc, 0, sizeof(mTc));
	memset(mAmbientOcclusionCacheBlocks, 0, sizeof(mAmbientOcclusionCacheBlocks));
}

void BlockTessellator::setRegion(BlockSource& region) {
	mRegion = &region;
}

BlockSource& BlockTessellator::getRegion() const {
	return *mRegion;
}

void BlockTessellator::setForceOpaque(bool forceOpaque) {
	mForceOpaque = forceOpaque;
}

void BlockTessellator::setRenderLayer(int layer) {
	mRenderingLayer = layer;
}

bool BlockTessellator::tessellateBlockInWorld(const Block& block, const BlockPos& pos, int data) {
	Color base = Color::fromARGB(block.getColor(*mRegion, pos, data));

	if(mUseOccluder) {
		BlockOccluder occluder = BlockOccluder(mBlockCache, block, pos, mCurrentShape, Facing::ALL_FACES, mRenderingGUI);
 		if(mSmoothLighting && !mRenderingGUI && Block::mLightEmission[block.mID] == 0 && Block::mTranslucency[block.mID] < 0.9f) {
 			return tessellateBlockInWorldWithAmbienceOcclusion(block, pos, data, base, &occluder);
 		}
 		else {
			return tessellateBlockInWorldFlat(block, pos, data, base, &occluder);
		}
	}
	else {
 		if(mSmoothLighting && !mRenderingGUI && Block::mLightEmission[block.mID] == 0 && Block::mTranslucency[block.mID] < 0.9f) {
 			return tessellateBlockInWorldWithAmbienceOcclusion(block, pos, data, base, nullptr);
 		} 
		else {
			return tessellateBlockInWorldFlat(block, pos, data, base, nullptr);
		}
	}
}

bool BlockTessellator::tessellateBlockInWorldFlat(const Block& block, const BlockPos& p, const int data, const Color& base, BlockOccluder* occluder){
	mApplyAmbientOcclusion = false;

	bool changed = false;

	bool e = Block::mLightEmission[block.mID] > 0;
	const float brightnessBelow = std::pow( e ? 0.875f : 0.5f, block.getBrightnessGamma());
	const float brightnessZ = std::pow(e ? 0.95f : 0.8f, block.getBrightnessGamma());
	const float brightnessX = std::pow(e ? 0.9f : 0.6f, block.getBrightnessGamma());

	auto centerColor = getLightColor(p);

	Color sidesBase = base;

	if (&block == Block::mGrass) {
		sidesBase = Color::WHITE; //do not tint anything except top
	}


	if (!occluder || !occluder->occludes(Facing::DOWN)) {
		auto belowPos = p.below();
		_tex1(toUV(mCurrentShape.min.y > 0 ? centerColor : getLightColor(belowPos)));
		mTessellator.color(sidesBase * brightnessBelow);
		renderFaceDown(block, p, _getTexture(block, Facing::DOWN, data));
		changed = true;
	}

	if (!occluder || !occluder->occludes(Facing::UP)) {
		auto abovePos = p.above();
		_tex1(toUV(mCurrentShape.max.y < 1 ? centerColor : getLightColor(abovePos)));
		mTessellator.color(base);
		renderFaceUp(block, p, _getTexture(block, Facing::UP, data));
		changed = true;
	}

	if(!occluder || !occluder->occludes(Facing::NORTH)) {
		auto northPos = p.north();
		_tex1(toUV(mCurrentShape.min.z > 0 ? centerColor : getLightColor(northPos)));

		mTessellator.color(sidesBase * brightnessZ);
		renderNorth(block, p, _getTexture(block, Facing::NORTH, data));
		changed = true;
	}

	if(!occluder || !occluder->occludes(Facing::SOUTH)) {
		auto southPos = p.south();
		_tex1(toUV(mCurrentShape.max.z < 1 ? centerColor : getLightColor(southPos)));
		mTessellator.color(sidesBase * brightnessZ);
		renderSouth(block, p, _getTexture(block, Facing::SOUTH, data));
		changed = true;
	}

	if(!occluder || !occluder->occludes(Facing::WEST)) {
		auto westPos = p.west();
		_tex1(toUV(mCurrentShape.min.x > 0 ? centerColor : getLightColor(westPos)));
		mTessellator.color(sidesBase * brightnessX);
		renderWest(block, p, _getTexture(block, Facing::WEST, data));
		changed = true;
	}

	if(!occluder || !occluder->occludes(Facing::EAST)) {
		auto eastPos = p.east();
		_tex1(toUV(mCurrentShape.max.x < 1 ? centerColor : getLightColor(eastPos)));
		mTessellator.color(sidesBase * brightnessX);
		renderEast(block, p, _getTexture(block, Facing::EAST, data));
		changed = true;
	}

	mFlipFace[0] = mFlipFace[1] = mFlipFace[2] = mFlipFace[3] = mFlipFace[4] = mFlipFace[5] = 0;

	return changed;
}

void BlockTessellator::tessellateInWorld(const Block& block, const BlockPos& pos, DataID data, const TextureUVCoordinateSet& fixedTexture){
	mUseFixedTexture = true;
	mFixedTexture = fixedTexture;
	tessellateInWorld(block, pos, data);
	mUseFixedTexture = false;
}

bool BlockTessellator::tessellateInWorld(const Block& block, const BlockPos& pos, DataID data, bool renderBothSides){

// HACK; fix when block models are available, you do not want to use Region in Block Tesselator!!!
	if(_tmpUseRegion) {
		mCurrentShape = block.getVisualShape(*mRegion, pos, mCurrentShape, false);
		//HACK 2: Grass blocks need to know about the region around it to determine the right texture permutation
		data = block.calcVariant(*mRegion, pos, data);
	}
	else {
		mCurrentShape = block.getVisualShape(data, mCurrentShape, false);
	}

	_tex1(toUV(getLightColor(pos)));

	BlockGraphics& blockGraphics = *BlockGraphics::mBlocks[block.getId()];
	BlockShape blockShape = blockGraphics.getBlockShape();

	bool ret = false;

	switch(blockShape) {
	case BlockShape::BLOCK:
	case BlockShape::BLOCK_HALF:
		ret = tessellateBlockInWorld(block, pos, data);
		break;
	case BlockShape::TOP_SNOW:
		ret = tessellateTopSnowInWorld(block, pos, data);
		break;
	case BlockShape::WATER:
		ret = tessellateLiquidInWorld(static_cast<const LiquidBlock&>(block), pos, data);
		break;
	case BlockShape::CACTUS:
		ret = tessellateCactusInWorld(block, pos, data);
		break;
	case BlockShape::CROSS_TEXTURE:
		ret = tessellateCrossInWorld(block, pos, data, renderBothSides);
		break;
	case BlockShape::CROSS_TEXTURE_POLY:
		ret = tessellateCrossPolyInWorld(block, pos, data, renderBothSides);
		break;
	case BlockShape::DOUBLE_PLANT: //BG_todo:	type not used 
		ret = tessellateDoublePlantInWorld(static_cast<const DoublePlantBlock&>(block), pos, data, false, renderBothSides);
		break;
	case BlockShape::DOUBLE_PLANT_POLY:
		ret = tessellateDoublePlantInWorld(static_cast<const DoublePlantBlock&>(block), pos, data, true, renderBothSides);
		break;
	case BlockShape::STEM:
		ret = tessellateStemInWorld(static_cast<const StemBlock&>(block), pos);
		break;
	case BlockShape::ROWS:
		ret = tessellateRowInWorld(block, pos, data);
		break;
	case BlockShape::TORCH:
		ret = tessellateTorchInWorld(block, pos, data);
		break;
	case BlockShape::FIRE:
		ret = tessellateFireInWorld(block, pos);
		break;
	case BlockShape::LADDER:
		ret = tessellateLadderInWorld(block, pos, data);
		break;
	case BlockShape::DOOR:
		ret = tessellateDoorInWorld(block, pos, data);
		break;
	case BlockShape::RAIL:
		ret = tessellateRailInWorld(static_cast<const BaseRailBlock&>(block), pos, data);
		break;
	case BlockShape::STAIRS:
		ret = tessellateStairsInWorld(static_cast<const StairBlock&>(block), pos, data);
		break;
	case BlockShape::FENCE:
		ret = tessellateFenceInWorld(static_cast<const FenceBlock&>(block), pos, data);
		break;
	case BlockShape::FENCE_GATE:
		ret = tessellateFenceGateInWorld(static_cast<const FenceGateBlock&>(block), pos, data);
		break;
	case BlockShape::WALL:
		ret = tessellateWallInWorld(static_cast<const WallBlock&>(block), pos, data);
		break;
	case BlockShape::IRON_FENCE:
		ret = tessellateThinFenceInWorld(static_cast<const ThinFenceBlock&>(block), pos, data);
		break;
	case BlockShape::TREE:
		ret = tessellateTreeInWorld(block, pos, data);
		break;
	case BlockShape::BED:
		ret = tessellateBedInWorld(block, pos, data);
		break;
	case BlockShape::DIODE:
		ret = tessellateDiodeInWorld(block, pos, data);
		break;
	case BlockShape::VINE:
		ret = tessellateVineInWorld(block, pos, data);
		break;
	case BlockShape::COCOA:
		ret = tessellateCocoaInWorld(static_cast<const CocoaBlock&>(block), pos, data);
		break;
	case BlockShape::LILYPAD:
		ret = tessellateLilypadInWorld(block, pos);
		break;
	case BlockShape::BREWING_STAND:
		ret = tessellateBrewingStandInWorld(static_cast<const BrewingStandBlock&>(block), pos, data);
		break;
	case BlockShape::PORTAL_FRAME:
		ret = tessellateEndPortalFrameInWorld(static_cast<const EndPortalFrameBlock&>(block), pos, data);
		break;
	case BlockShape::FLOWER_POT:
		ret = tessellateFlowerPotInWorld(static_cast<const FlowerPotBlock&>(block), pos, data);
		break;
	case BlockShape::ANVIL:
		ret = tessellateAnvilInWorld(static_cast<const AnvilBlock&>(block), pos, data);
		break;
	case BlockShape::STRUCTURE_VOID:
		ret = tessellateStructureVoidInWorld(block, pos, data);
		break;
	case BlockShape::LEVER:
		ret = tessellateLeverInWorld(block, pos, data);
		break;
	case BlockShape::RED_DUST:
		ret = tessellateDustInWorld(block, pos, data);
		break;
	case BlockShape::TRIPWIRE_HOOK: 
		ret = tessellateTripwireHookInWorld(block, pos, data);
		break;
	case BlockShape::TRIPWIRE: 
		ret = tessellateTripwireInWorld(block, pos, data);
		break;
	case BlockShape::REPEATER:
		ret = tessellateRepeaterInWorld(block, pos, data);
		break;
	case BlockShape::COMPARATOR:
		ret = tessellateComparatorInWorld(block, pos, data);
		break;
	case BlockShape::CAULDRON:
		ret = tessellateCauldronInWorld(block, pos, data);
		break;
	case BlockShape::HOPPER:
		ret = tessellateHopperInWorld(block, pos, data);
		break;
	case BlockShape::SLIME_BLOCK:
		ret = tessellateSlimeBlockInWorld(block, pos, data);
		break;
	case BlockShape::PISTON:
		ret = tessellatePistonInWorld(block, pos, data);
		break;
	case BlockShape::BEACON:
		ret = tessellateBeaconInWorld(block, pos, data);
		break;
	case BlockShape::CHORUS_PLANT:
		ret = tessellateChorusPlantInWorld(static_cast<const ChorusPlantBlock&>(block), pos, data);
		break;
	case BlockShape::CHORUS_FLOWER:
		ret = tessellateChorusFlowerInWorld(block, pos, data);
		break;
	case BlockShape::DRAGON_EGG:
		ret = tessellateDragonEgg(block, pos, data, false);
		break;
	case BlockShape::END_PORTAL:
		ret = tessellateEndPortalInWorld(block, pos, data);
		break;
	case BlockShape::END_ROD:
		ret = tessellateEndRodInWorld(block, pos, data);
		break;
	case BlockShape::END_GATEWAY:
		ret = tessellateEndGatewayInWorld(block, pos, data);
		break;
	default:
		ret = false;
		break;
	}

	if(!mRenderingExtra && block.canHaveExtraData()) {
		tessellateExtraDataInWorld(block, pos);
	}

	return ret;

}

bool BlockTessellator::tessellateAnvilInWorld(const AnvilBlock& block, const BlockPos& pos) {
	return tessellateAnvilInWorld(block, pos, mRegion->getData(pos));
}

bool BlockTessellator::tessellateAnvilInWorld(const AnvilBlock& block, const BlockPos& pos, int data) {
	mTessellator.color(block.getColor(data));
	return tessellateAnvilInWorld(block, pos, data, false);
}

bool BlockTessellator::tessellateAnvilInWorld(const AnvilBlock& block, const BlockPos& pos, int data, bool render) {
	mTessellator.color(block.getColor(data));
	int facing = data & 3;
	bool rotate = false;
	float bottom = 0.0f;

	//Because the flipface flags get reset after each call of tessellateBlockInWorld inside of each
	// tessellateAnvilPiece,
	//I store faces here and pass it to the function. It's not super clean, but wasn't sure if creating a cache for this
	// one object was a better response.
	const int FACE_COUNT = 6;
	int faces[FACE_COUNT] = {
		0, 0, 0, 0, 0, 0
	};

	switch (facing) {
	case Direction::NORTH:
		faces[Facing::EAST] = FLIP_CW;
		faces[Facing::WEST] = FLIP_CCW;
		break;
	case Direction::SOUTH:
		faces[Facing::EAST] = FLIP_CCW;
		faces[Facing::WEST] = FLIP_CW;
		faces[Facing::UP] = FLIP_180;
		faces[Facing::DOWN] = FLIP_180;
		break;
	case Direction::WEST:
		faces[Facing::NORTH] = FLIP_CW;
		faces[Facing::SOUTH] = FLIP_CCW;
		faces[Facing::UP] = FLIP_CCW;
		faces[Facing::DOWN] = FLIP_CW;
		rotate = true;
		break;
	case Direction::EAST:
		faces[Facing::NORTH] = FLIP_CCW;
		faces[Facing::SOUTH] = FLIP_CW;
		faces[Facing::UP] = FLIP_CW;
		faces[Facing::DOWN] = FLIP_CCW;
		rotate = true;
		break;

	default:
		break;
	}

	bottom = tessellateAnvilPiece(block, pos, AnvilPart::Base, bottom, 12.f / 16.f, 4.f / 16.f, 12.f / 16.f, rotate, render, AnvilBlock::getDataForSide(data), faces, FACE_COUNT);
	bottom = tessellateAnvilPiece(block, pos, AnvilPart::Joint, bottom, 8.f / 16.f, 1.f / 16.f, 10.f / 16.f, rotate, render, AnvilBlock::getDataForSide(data), faces, FACE_COUNT);
	bottom = tessellateAnvilPiece(block, pos, AnvilPart::Column, bottom, 4.f / 16.f, 5.f / 16.f, 8.f / 16.f, rotate, render, AnvilBlock::getDataForSide(data), faces, FACE_COUNT);
	bottom = tessellateAnvilPiece(block, pos, AnvilPart::Top, bottom, 10.f / 16.f, (6.f / 16.f) - TEXTURE_OFFSET, 16.f / 16.f, rotate, render, data, faces, FACE_COUNT);

	mCurrentShape.set(0.f, 0.f, 0.f, 1.f, 1.f, 1.f);

	return true;
}

float BlockTessellator::tessellateAnvilPiece(const AnvilBlock& block, const BlockPos& p, AnvilPart part, float bottom, float width, float height, float length, bool rotate, bool render, int data, int* faces, int num_faces) {
	if (rotate) {
		float swap = width;
		width = length;
		length = swap;
	}

	for (int i = 0; i < num_faces; ++i) {
		mFlipFace[i] = faces[i];
	}

	width /= 2;
	length /= 2;

	mCurrentShape.set(0.5f - width, bottom, 0.5f - length, 0.5f + width, bottom + height, 0.5f + length);

	BlockGraphics& blockGraphics = *BlockGraphics::mBlocks[block.getId()];
	int variant = block.getVariant(data);
	if (render) {
		renderFaceDown(block, p, blockGraphics.getTexture(Facing::DOWN, variant));
		renderFaceUp(block, p, blockGraphics.getTexture(Facing::UP, variant));
		renderNorth(block, p, blockGraphics.getTexture(Facing::NORTH, variant));
		renderSouth(block, p, blockGraphics.getTexture(Facing::SOUTH, variant));
		renderWest(block, p, blockGraphics.getTexture(Facing::WEST, variant));
		renderEast(block, p, blockGraphics.getTexture(Facing::EAST, variant));
	} else {
		mRenderingData = data;
		tessellateBlockInWorld(block, p, data);
	}

	return bottom + height;
}

bool BlockTessellator::tessellateTopSnowInWorld(const Block& block, const BlockPos& pos, DataID data) {

	if (mRenderingLayer != BlockRenderLayer::RENDERLAYER_OPAQUE) {

		// Find out if this top snow is covering up something, if so we need to tessellate that separately.
		FullBlock recoveredBlock = TopSnowBlock::dataIDToRecoverableFullBlock(*mRegion, pos, data);
		if (!recoveredBlock.isAir()) {
			if (mRenderingLayer == Block::mBlocks[recoveredBlock.id]->getRenderLayer()) {
				return tessellateInWorld(*Block::mBlocks[recoveredBlock.id], pos, recoveredBlock.data);
			}
		}

		return false;
	}

	return tessellateBlockInWorld(block, pos, data);
}

bool BlockTessellator::tessellateExtraDataInWorld(const Block& block, const BlockPos& pos) {

	if (mRenderingLayer != BlockRenderLayer::RENDERLAYER_OPAQUE) {
		uint16_t extraData = mRegion->getExtraData(pos);
		if (extraData != 0) {
			const Block* pBlock = Block::mBlocks[extraData & 0xFF];
			if (pBlock != nullptr && pBlock->getRenderLayer() == mRenderingLayer) {
				mRenderingExtra = true;
				bool ret = tessellateInWorld(*pBlock, pos, extraData >> 8);
				mRenderingExtra = false;

				return ret;
			}
		}
	}

	return false;
}

bool BlockTessellator::tessellateTorchInWorld(const Block& block, const BlockPos& pos, DataID data){
	auto dir = data;

	_tex1(toUV(getLightColor(pos)));
	mTessellator.color(Color::WHITE);

	float r = 0.40f;
	float r2 = 0.5f - r;
	float h = 0.20f;

	Vec3 p = pos;

	if (dir == 1) {
		tessellateTorch(block, Vec3(p.x - r2, p.y + h, p.z), -r, 0);
	} else if (dir == 2) {
		tessellateTorch(block, Vec3(p.x + r2, p.y + h, p.z), +r, 0);
	} else if (dir == 3) {
		tessellateTorch(block, Vec3(p.x, p.y + h, p.z - r2), 0, -r);
	} else if (dir == 4) {
		tessellateTorch(block, Vec3(p.x, p.y + h, p.z + r2), 0, +r);
	} else {
		tessellateTorch(block, Vec3(p), 0, 0);
	}

	// Light spawn testing
	if (mUnrealActor != nullptr) {
		UPointLightComponent* light = NewObject<UPointLightComponent>(mUnrealActor, FName(("Light_" + pos.toString()).c_str()));
		light->SetMobility(EComponentMobility::Stationary);
		light->RegisterComponent();
		light->Intensity = 7000;
		light->bVisible = true;
		light->SetLightColor(FLinearColor(1.0f, 0.5f, 0.2f));
		const float scale = 75.0f;
		light->SetRelativeLocation(FVector((pos.x + 0.5f) * scale, (pos.z + 0.5f) * scale, (pos.y + 0.85f) * scale));
		light->SetCastShadows(false);
		light->SetAttenuationRadius(1000);
		light->MaxDrawDistance = 5000.0f;
		light->MaxDistanceFadeRange = 500.0f;
		light->AttachToComponent(mUnrealActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	}

	return true;
}

bool BlockTessellator::tessellateFireInWorld(const Block& block, BlockPos p) {
	const BlockGraphics& blockGraphics = *BlockGraphics::mBlocks[block.getId()];
	const TextureUVCoordinateSet& firstTex = blockGraphics.getTexture(Facing::UP);
	const TextureUVCoordinateSet& secondTex = blockGraphics.getTexture(Facing::DOWN);

	float u0 = firstTex._u0;
	float v0 = firstTex._v0;
	float u1 = firstTex._u1;
	float v1 = firstTex._v1;
	float h = 1.4f;

	mTessellator.color(Color::WHITE);
	_tex1(toUV(getLightColor(p)));

	auto& fireBlock = (const FireBlock&)*Block::mFire;

	if (mRegion->isSolidBlockingBlock(p.below()) || fireBlock.canBurn(*mRegion, p.below())) {
		float x0 = p.x + 0.5f + 0.2f;
		float x1 = p.x + 0.5f - 0.2f;
		float z0 = p.z + 0.5f + 0.2f;
		float z1 = p.z + 0.5f - 0.2f;

		float x0_ = p.x + 0.5f - 0.3f;
		float x1_ = p.x + 0.5f + 0.3f;
		float z0_ = p.z + 0.5f - 0.3f;
		float z1_ = p.z + 0.5f + 0.3f;

		mTessellator.vertexUV(x0_, (float)p.y + h, p.z + 1.0f, u1, v0);
		mTessellator.vertexUV(x0, p.y + 0.0f, p.z + 1.0f, u1, v1);
		mTessellator.vertexUV(x0, p.y + 0.0f, p.z + 0.0f, u0, v1);
		mTessellator.vertexUV(x0_, (float) p.y + h, p.z + 0.0f, u0, v0);

		mTessellator.vertexUV(x1_, (float) p.y + h, p.z + 0.0f, u1, v0);
		mTessellator.vertexUV(x1, p.y + 0.0f, p.z + 0.0f, u1, v1);
		mTessellator.vertexUV(x1, p.y + 0.0f, p.z + 1.0f, u0, v1);
		mTessellator.vertexUV(x1_, (float) p.y + h, p.z + 1.0f, u0, v0);

		u0 = secondTex._u0;
		v0 = secondTex._v0;
		u1 = secondTex._u1;
		v1 = secondTex._v1;

		mTessellator.vertexUV(p.x + 1.0f, (float) p.y + h, z1_, u1, v0);
		mTessellator.vertexUV(p.x + 1.0f, p.y + 0.0f, z1, u1, v1);
		mTessellator.vertexUV(p.x + 0.0f, p.y + 0.0f, z1, u0, v1);
		mTessellator.vertexUV(p.x + 0.0f, (float) p.y + h, z1_, u0, v0);

		mTessellator.vertexUV(p.x + 0.0f, (float) p.y + h, (float) z0_, u1, v0);
		mTessellator.vertexUV(p.x + 0.0f, p.y + 0.0f, z0, u1, v1);
		mTessellator.vertexUV(p.x + 1.0f, p.y + 0.0f, z0, u0, v1);
		mTessellator.vertexUV(p.x + 1.0f, (float) p.y + h, (float) z0_, u0, v0);

		x0 = p.x + 0.5f - 0.5f;
		x1 = p.x + 0.5f + 0.5f;
		z0 = p.z + 0.5f - 0.5f;
		z1 = p.z + 0.5f + 0.5f;

		x0_ = p.x + 0.5f - 0.4f;
		x1_ = p.x + 0.5f + 0.4f;
		z0_ = p.z + 0.5f - 0.4f;
		z1_ = p.z + 0.5f + 0.4f;

		mTessellator.vertexUV(x0_, (float) p.y + h, p.z + 0.0f, u0, v0);
		mTessellator.vertexUV(x0, p.y + 0.0f, p.z + 0.0f, u0, v1);
		mTessellator.vertexUV(x0, p.y + 0.0f, p.z + 1.0f, u1, v1);
		mTessellator.vertexUV(x0_, (float) p.y + h, p.z + 1.0f, u1, v0);

		mTessellator.vertexUV(x1_, (float) p.y + h, p.z + 1.0f, u0, v0);
		mTessellator.vertexUV(x1, p.y + 0.0f, p.z + 1.0f, u0, v1);
		mTessellator.vertexUV(x1, p.y + 0.0f, p.z + 0.0f, u1, v1);
		mTessellator.vertexUV(x1_, (float) p.y + h, p.z + 0.0f, u1, v0);

		u0 = firstTex._u0;
		v0 = firstTex._v0;
		u1 = firstTex._u1;
		v1 = firstTex._v1;

		mTessellator.vertexUV(p.x + 0.0f, p.y + h, z1_, u0, v0);
		mTessellator.vertexUV(p.x + 0.0f, (float)p.y + 0.0f, z1, u0, v1);
		mTessellator.vertexUV(p.x + 1.0f, (float) p.y + 0.0f, z1, u1, v1);
		mTessellator.vertexUV(p.x + 1.0f, p.y + h, z1_, u1, v0);

		mTessellator.vertexUV(p.x + 1.0f, p.y + h, z0_, u0, v0);
		mTessellator.vertexUV(p.x + 1.0f, (float) p.y + 0.0f, z0, u0, v1);
		mTessellator.vertexUV(p.x + 0.0f, (float) p.y + 0.0f, z0, u1, v1);
		mTessellator.vertexUV(p.x + 0.0f, p.y + h, z0_, u1, v0);
	} else {
		float r = 0.2f;
		float yo = 1 / 16.0f;
		if (((p.x + p.y + p.z) & 1) == 1) {
			u0 = secondTex._u0;
			v0 = secondTex._v0;
			u1 = secondTex._u1;
			v1 = secondTex._v1;
		}

		if (((p.x / 2 + p.y / 2 + p.z / 2) & 1) == 1) {
			float tmp = u1;
			u1 = u0;
			u0 = tmp;
		}

		if (fireBlock.canBurn(*mRegion, p.west())) {
			mTessellator.vertexUV(p.x + r, p.y + h + yo, p.z + 1.0f, u1, v0);
			mTessellator.vertexUV(p.x + 0.0f, p.y + 0 + yo, p.z + 1.0f, u1, v1);
			mTessellator.vertexUV(p.x + 0.0f, p.y + 0 + yo, p.z + 0.0f, u0, v1);
			mTessellator.vertexUV(p.x + r, p.y + h + yo, p.z + 0.0f, u0, v0);

			mTessellator.vertexUV(p.x + r, p.y + h + yo, p.z + 0.0f, u0, v0);
			mTessellator.vertexUV(p.x + 0.0f, p.y + 0 + yo, p.z + 0.0f, u0, v1);
			mTessellator.vertexUV(p.x + 0.0f, p.y + 0 + yo, p.z + 1.0f, u1, v1);
			mTessellator.vertexUV(p.x + r, p.y + h + yo, p.z + 1.0f, u1, v0);
		}

		if (fireBlock.canBurn(*mRegion, p.east())) {
			mTessellator.vertexUV(p.x + 1.0f - (float)r, p.y + h + yo, p.z + 0.0f, u0, v0);
			mTessellator.vertexUV(p.x + 1.0f - 0, p.y + 0 + yo, p.z + 0.0f, u0, v1);
			mTessellator.vertexUV(p.x + 1.0f - 0, p.y + 0 + yo, p.z + 1.0f, u1, v1);
			mTessellator.vertexUV(p.x + 1.0f - (float)r, p.y + h + yo, p.z + 1.0f, u1, v0);

			mTessellator.vertexUV(p.x + 1.0f - (float)r, p.y + h + yo, p.z + 1.0f, u1, v0);
			mTessellator.vertexUV(p.x + 1.0f - 0, p.y + 0 + yo, p.z + 1.0f, u1, v1);
			mTessellator.vertexUV(p.x + 1.0f - 0, p.y + 0 + yo, p.z + 0.0f, u0, v1);
			mTessellator.vertexUV(p.x + 1.0f - (float)r, p.y + h + yo, p.z + 0.0f, u0, v0);
		}

		if (fireBlock.canBurn(*mRegion, p.north())) {
			mTessellator.vertexUV(p.x + 0.0f, p.y + h + yo, p.z + r, u1, v0);
			mTessellator.vertexUV(p.x + 0.0f, p.y + 0 + yo, p.z + 0.0f, u1, v1);
			mTessellator.vertexUV(p.x + 1.0f, p.y + 0 + yo, p.z + 0.0f, u0, v1);
			mTessellator.vertexUV(p.x + 1.0f, p.y + h + yo, p.z + r, u0, v0);

			mTessellator.vertexUV(p.x + 1.0f, p.y + h + yo, p.z + r, u0, v0);
			mTessellator.vertexUV(p.x + 1.0f, p.y + 0 + yo, p.z + 0.0f, u0, v1);
			mTessellator.vertexUV(p.x + 0.0f, p.y + 0 + yo, p.z + 0.0f, u1, v1);
			mTessellator.vertexUV(p.x + 0.0f, p.y + h + yo, p.z + r, u1, v0);
		}

		if (fireBlock.canBurn(*mRegion, p.south())) {
			mTessellator.vertexUV(p.x + 1.0f, p.y + h + yo, p.z + 1.0f - (float)r, u0, v0);
			mTessellator.vertexUV(p.x + 1.0f, p.y + 0 + yo, p.z + 1.0f - 0, u0, v1);
			mTessellator.vertexUV(p.x + 0.0f, p.y + 0 + yo, p.z + 1.0f - 0, u1, v1);
			mTessellator.vertexUV(p.x + 0.0f, p.y + h + yo, p.z + 1.0f - (float)r, u1, v0);

			mTessellator.vertexUV(p.x + 0.0f, p.y + h + yo, p.z + 1.0f - (float)r, u1, v0);
			mTessellator.vertexUV(p.x + 0.0f, p.y + 0 + yo, p.z + 1.0f - 0, u1, v1);
			mTessellator.vertexUV(p.x + 1.0f, p.y + 0 + yo, p.z + 1.0f - 0, u0, v1);
			mTessellator.vertexUV(p.x + 1.0f, p.y + h + yo, p.z + 1.0f - (float)r, u0, v0);
		}

		if (fireBlock.canBurn(*mRegion, p.above())) {
			float x0 = p.x + 0.5f + 0.5f;
			float x1 = p.x + 0.5f - 0.5f;
			float z0 = p.z + 0.5f + 0.5f;
			float z1 = p.z + 0.5f - 0.5f;

			float x0_ = p.x + 0.5f - 0.5f;
			float x1_ = p.x + 0.5f + 0.5f;
			float z0_ = p.z + 0.5f - 0.5f;
			float z1_ = p.z + 0.5f + 0.5f;

			u0 = firstTex._u0;
			v0 = firstTex._v0;
			u1 = firstTex._u1;
			v1 = firstTex._v1;

			p.y += 1;
			h = -0.2f;

			if (((p.x + p.y + p.z) & 1) == 0) {
				mTessellator.vertexUV(x0_, (float)p.y + h, p.z + 0.0f, u1, v0);
				mTessellator.vertexUV(x0, p.y + 0.0f, p.z + 0.0f, u1, v1);
				mTessellator.vertexUV(x0, p.y + 0.0f, p.z + 1.0f, u0, v1);
				mTessellator.vertexUV(x0_, (float)p.y + h, p.z + 1.0f, u0, v0);

				u0 = secondTex._u0;
				v0 = secondTex._v0;
				u1 = secondTex._u1;
				v1 = secondTex._v1;

				mTessellator.vertexUV(x1_, (float)p.y + h, p.z + 1.0f, u1, v0);
				mTessellator.vertexUV(x1, p.y + 0.0f, p.z + 1.0f, u1, v1);
				mTessellator.vertexUV(x1, p.y + 0.0f, p.z + 0.0f, u0, v1);
				mTessellator.vertexUV(x1_, (float)p.y + h, p.z + 0.0f, u0, v0);
			} else {

				mTessellator.vertexUV(p.x + 0.0f, p.y + h, z1_, u1, v0);
				mTessellator.vertexUV(p.x + 0.0f, p.y + 0.0f, z1, u1, v1);
				mTessellator.vertexUV(p.x + 1.0f, p.y + 0.0f, z1, u0, v1);
				mTessellator.vertexUV(p.x + 1.0f, (float)p.y + h, z1_, u0, v0);

				u0 = secondTex._u0;
				v0 = secondTex._v0;
				u1 = secondTex._u1;
				v1 = secondTex._v1;

				mTessellator.vertexUV(p.x + 1.0f, (float)p.y + h, z0_, u1, v0);
				mTessellator.vertexUV(p.x + 1.0f, p.y + 0.0f, z0, u1, v1);
				mTessellator.vertexUV(p.x + 0.0f, p.y + 0.0f, z0, u0, v1);
				mTessellator.vertexUV(p.x + 0.0f, p.y + h, z0_, u0, v0);
			}
		}
	}

	return true;
}

bool BlockTessellator::tessellateFlowerPotInWorld(const FlowerPotBlock& block, const BlockPos& pos, int data) {
	//Rendering the outside of the pot first
	tessellateBlockInWorld(block, pos, data);

	//Rendering the inside of the pot.
	float halfWidth = (6.0f / 16.0f) / 2 - 0.001f;
	mCurrentShape.set(Vec3(0.3125f, 0.0f, 0.3125f), Vec3(0.6875f, 0.375f, 0.6875f));
	renderFaceUp(block, Vec3((float)pos.x, (float)pos.y - .5f + halfWidth + 3.0f / 16.0f, (float)pos.z), _getTexture(*Block::mDirt, 1, data));

	//TODO: rherlitz
// 	BlockEntity* te = mRegion->getBlockEntity(pos);
// 	if (te != nullptr && te->getType() == BlockEntityType::FlowerPot) {
// 		//Check for existence of the plant to draw
// 		FlowerPotBlockEntity* fpte = (FlowerPotBlockEntity*)te;
// 		const Block* plant = fpte->getPlantItem();
// 		if (plant != nullptr) {
// 			BlockGraphics& blockGraphics = *BlockGraphics::mBlocks[plant->getId()];
// 			int itemData = fpte->getItemData();
// 			BlockShape blockShape = blockGraphics.getBlockShape();
// 
// 			//Start drawing the plant at an offset, near the top of the pot.
// 			//Unnecessary math just for code consistency with java version.
// 			float xOff = 0;
// 			float yOff = 4;
// 			float zOff = 0;
// 
// 			mTessellator.color(plant->getColor(itemData));
// 			_tex1(toUV(getLightColor(pos)));
// 
// 			mTessellator.addOffset(xOff / 16.f, yOff / 16.f, zOff / 16.f);
// 
// 			//Draw the potted object in the right position
// 			if (blockShape == BlockShape::CROSS_TEXTURE_POLY || blockShape == BlockShape::CROSS_TEXTURE) {
// 				const TextureUVCoordinateSet& tex = mUseFixedTexture ? mFixedTexture : blockGraphics.getTexture(Facing::DOWN, plant->getVariant(itemData));
// 				tessellateCrossTexture(tex, Vec3(pos), false, block, .75f);
// 			} else if( blockShape == BlockShape::CACTUS) {	//This texture is off from the java version. Not sure why.
// 				float halfSize = 0.25f / 2.0f;
// 
// 				//rendering Cactus block
// 				mCurrentShape.set(0.5f - halfSize, 0.0f, 0.5f - halfSize, 0.5f + halfSize, 0.75f, 0.5f + halfSize);
// 				tessellateBlockInWorld(*plant, pos, data);
// 
// 				//Restoring defaults
// 				mCurrentShape.set(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
// 			}
// 
// 			//Undoing the offset
// 			mTessellator.addOffset(-xOff / 16.f, -yOff / 16.f, -zOff / 16.f);
// 		}
// 	}

	return true;
}

bool BlockTessellator::tessellateLadderInWorld(const Block& block, const BlockPos& p, DataID data) {
	const BlockGraphics& blockGraphics = *BlockGraphics::mBlocks[block.getId()];
	const TextureUVCoordinateSet& tex = mUseFixedTexture ? mFixedTexture : blockGraphics.getTexture(Facing::DOWN);

	_tex1(toUV(getLightColor(p)));
	mTessellator.color(Color::WHITE);

	float u0 = tex._u0;
	float u1 = tex._u1;
	float v0 = tex._v0;
	float v1 = tex._v1;

	FacingID face = data;

	float o = 0 / 16.0f;
	float r = 0.05f;
	if (face == Facing::EAST) {
		mTessellator.vertexUV(p.x + r, p.y + 1 + o, p.z + 1 + o, u0, v0);
		mTessellator.vertexUV(p.x + r, p.y + 0 - o, p.z + 1 + o, u0, v1);
		mTessellator.vertexUV(p.x + r, p.y + 0 - o, p.z + 0 - o, u1, v1);
		mTessellator.vertexUV(p.x + r, p.y + 1 + o, p.z + 0 - o, u1, v0);
	}

	if (face == Facing::WEST) {
		mTessellator.vertexUV(p.x + 1 - r, p.y + 0 - o, p.z + 1 + o, u1, v1);
		mTessellator.vertexUV(p.x + 1 - r, p.y + 1 + o, p.z + 1 + o, u1, v0);
		mTessellator.vertexUV(p.x + 1 - r, p.y + 1 + o, p.z + 0 - o, u0, v0);
		mTessellator.vertexUV(p.x + 1 - r, p.y + 0 - o, p.z + 0 - o, u0, v1);
	}

	if (face == Facing::SOUTH) {
		mTessellator.vertexUV(p.x + 1 + o, p.y + 0 - o, p.z + r, u1, v1);
		mTessellator.vertexUV(p.x + 1 + o, p.y + 1 + o, p.z + r, u1, v0);
		mTessellator.vertexUV(p.x + 0 - o, p.y + 1 + o, p.z + r, u0, v0);
		mTessellator.vertexUV(p.x + 0 - o, p.y + 0 - o, p.z + r, u0, v1);
	}

	if (face == Facing::NORTH) {
		mTessellator.vertexUV(p.x + 1 + o, p.y + 1 + o, p.z + 1 - r, u0, v0);
		mTessellator.vertexUV(p.x + 1 + o, p.y + 0 - o, p.z + 1 - r, u0, v1);
		mTessellator.vertexUV(p.x + 0 - o, p.y + 0 - o, p.z + 1 - r, u1, v1);
		mTessellator.vertexUV(p.x + 0 - o, p.y + 1 + o, p.z + 1 - r, u1, v0);
	}

	return true;
}

Color BlockTessellator::_getBlockColor(const BlockPos& p, const Block& block) {

	Color result = Color::fromARGB(block.getColor(*mRegion, p, mRegion->getData(p)));
	if (block.isSeasonTinted(*mRegion, p)) {
		result.b = 1.0f;
	}
	return result;
}

Color BlockTessellator::_getBlockColor(const BlockPos& p, const Block& block, DataID data) {

	Color result = Color::fromARGB(block.getColor(*mRegion, p, data));
	if (block.isSeasonTinted(*mRegion, p)) {
		result.b = 1.0f;
	}
	return result;
}

Vec3 BlockTessellator::_preparePolyCross(const BlockPos& p, const Block& block) {
	Color blockColor = _getBlockColor(p, block);
	blockColor.a = 1.0f;
	mTessellator.color(blockColor);
	_tex1(toUV(getLightColor(p)));

	return (&block == Block::mTallgrass || &block == Block::mYellowFlower || &block == Block::mRedFlower) ?
		block.randomlyModifyPosition(p) : Vec3(p);
}

bool BlockTessellator::tessellateCrossInWorld(const Block& block, const BlockPos& p, DataID data, bool renderBothSides ) {
	Vec3 v = _preparePolyCross(p, block);

	tessellateCrossTexture(block, data, v, renderBothSides);
	return true;
}

bool BlockTessellator::tessellateCrossPolyInWorld(const Block& block, const BlockPos& p, DataID data, bool renderBothSides) {
	Vec3 t = _preparePolyCross(p, block);
	const TextureUVCoordinateSet& tex = BlockGraphics::mBlocks[block.getId()]->getTexture(Facing::DOWN, block.getVariant(data));
	tessellateCrossPolyTexture(tex, t, renderBothSides, block);
	
	return true;
}

bool BlockTessellator::tessellateDoublePlantInWorld(const DoublePlantBlock& block, const BlockPos& p, DataID data, bool preferPoly, bool renderBothSides) {
	static const FacingID SUNFLOWER_ADDITIONAL = Facing::NORTH;
	static const FacingID PLANT_TOP = Facing::UP;
	static const FacingID PLANT_BOTTOM = Facing::DOWN;
	const BlockGraphics& blockGraphics = *BlockGraphics::mBlocks[block.getId()];

	// double plant color
	Color blockColor = _getBlockColor(p, block, data);
	blockColor.a = 1.0f;

	mTessellator.color(blockColor);
	_tex1(toUV(getLightColor(p)));

	int32_t seed;
	Vec3 v = block.randomlyModifyPosition(p, seed);

	int type = ((const DoublePlantBlock*)Block::mDoublePlant)->getType(*mRegion, p);
	bool isTop = Block::mDoublePlant->getBlockState(BlockState::UpperBlockBit).getBool(data);
	const TextureUVCoordinateSet& stem = blockGraphics.getTexture(isTop ? PLANT_TOP : PLANT_BOTTOM, type);
	if(preferPoly) {
		tessellateCrossPolyTexture(stem, v, renderBothSides, block);
	} else {
		tessellateCrossTexture(stem, v, renderBothSides, block);
	}

	if(isTop && type == enum_cast(DoublePlantType::Sunflower)) {
		float angle = Math::cos(seed * .8f) * PI * 0.1f;
		const float bOff = -0.3f;
		const float tOff = 0.05f;
		const static int FRONT = Facing::DOWN;
		const static int BACK = Facing::UP;
		const TextureUVCoordinateSet& sunflowerFront = blockGraphics.getTexture(SUNFLOWER_ADDITIONAL, FRONT);
		const TextureUVCoordinateSet& sunflowerBack = blockGraphics.getTexture(SUNFLOWER_ADDITIONAL, BACK);
		BlockTextureTessellator::render(mTessellator, Vec3(v.x + 0.5f, v.y, v.z + tOff), sunflowerFront, true, 0, angle, Vec3(0, 0, bOff));
		BlockTextureTessellator::render(mTessellator, Vec3(v.x - 0.5f, v.y, v.z + tOff * 1.3f), sunflowerBack, true, 0, angle + 180.0f, Vec3(0, 0, bOff));
	}

//  const TextureUVCoordinateSet& tex = mUseFixedTexture ? mFixedTexture : block.getTexture(0);
//  tessellateCrossPolyTexture(tex, getData(x, y, z), xt, yt, zt);
	return true;
}

bool BlockTessellator::tessellateStemInWorld(const StemBlock& block, const BlockPos& p) {
	Color blockColor = _getBlockColor(p, block);
	blockColor.a = 1.0f;
	mTessellator.color(blockColor);
	_tex1(toUV(getLightColor(p)));

	int dir = block.getConnectDir(*mRegion, p);
	if (dir < 0) {
		tessellateStemTexture(block, getData(p), mCurrentShape.max.y, float(p.x), float(p.y - 1 / 16.0f), float(p.z));
	} else {
		tessellateStemTexture(block, getData(p), 0.5f, float(p.x), float(p.y - 1 / 16.0f), float(p.z));
		tessellateStemDirTexture(block, getData(p), dir, mCurrentShape.max.y, Vec3(float(p.x), float(p.y - 1 / 16.0f), float(p.z)));
	}

	return true;
}

bool BlockTessellator::tessellateLilypadInWorld(const Block& block, const BlockPos& p) {
	const BlockGraphics& blockGraphics = *BlockGraphics::mBlocks[block.getId()];
	const auto& tex = mUseFixedTexture ? mFixedTexture : blockGraphics.getTexture(Facing::UP);

	float h = 0.25f / 16.0f;

	float u0 = tex._u0;
	float v0 = tex._v0;
	float u1 = tex._u1;
	float v1 = tex._v1;

	int32_t seed = (p.x * 3129871) ^ (p.z * 116129781l) ^ (p.y);
	seed = seed * seed * 42317861 + seed * 11;

	int dir = (int)((seed >> 16) & 0x3);

	float xx = p.x + 0.5f;
	float zz = p.z + 0.5f;
	float c = ((dir & 1) * 0.5f) * (1 - dir / 2 % 2 * 2);
	float s = (((dir + 1) & 1) * 0.5f) * (1 - (dir + 1) / 2 % 2 * 2);

	_tex1(toUV(getLightColor(p)));

	Color blockColor = Color::fromARGB(block.getColor(*mRegion, p));
	mTessellator.color(blockColor);
	mTessellator.vertexUV(xx + c - s, p.y + h, zz + c + s, u0, v0);
	mTessellator.vertexUV(xx + c + s, p.y + h, zz - c + s, u1, v0);
	mTessellator.vertexUV(xx - c + s, p.y + h, zz - c - s, u1, v1);
	mTessellator.vertexUV(xx - c - s, p.y + h, zz + c - s, u0, v1);

	mTessellator.color(blockColor * (1.0f / 16.0f));
	mTessellator.vertexUV(xx - c - s, p.y + h - 0.01f, zz + c - s, u0, v1);
	mTessellator.vertexUV(xx - c + s, p.y + h - 0.01f, zz - c - s, u1, v1);
	mTessellator.vertexUV(xx + c + s, p.y + h - 0.01f, zz - c + s, u1, v0);
	mTessellator.vertexUV(xx + c - s, p.y + h - 0.01f, zz + c + s, u0, v0);

	return true;
}

bool BlockTessellator::tessellateBrewingStandInWorld(const BrewingStandBlock& block, const BlockPos& pos, int data) {
	// Center post
	mCurrentShape.set(7.0f / 16.0f, 0, 7.0f / 16.0f, 9.0f / 16.0f, 14.0f / 16.0f, 9.0f / 16.0f);
	tessellateBlockInWorld(block, pos, data);

	const BlockGraphics& blockGraphics = *BlockGraphics::mBlocks[block.getId()];
	// Base blocks

	// If this is being called and a fixed texture was already set (ie. destroy cracks), don't overwrite it
	bool hadFixedTexture = mUseFixedTexture;
	const static FacingID BREWSTAND = Facing::UP;
	const static FacingID BREWSTAND_BASE = Facing::DOWN;
	
	mFixedTexture = hadFixedTexture ? mFixedTexture : blockGraphics.getTexture(BREWSTAND_BASE);

	mUseFixedTexture = true;
	mCurrentShape.set(9.0f / 16.0f, 0, 5.0f / 16.0f, 15.0f / 16.0f, 2 / 16.0f, 11.0f / 16.0f);
	tessellateBlockInWorld(block, pos, data);
	mCurrentShape.set(2.0f / 16.0f, 0, 1.0f / 16.0f, 8.0f / 16.0f, 2 / 16.0f, 7.0f / 16.0f);
	tessellateBlockInWorld(block, pos, data);
	mCurrentShape.set(2.0f / 16.0f, 0, 9.0f / 16.0f, 8.0f / 16.0f, 2 / 16.0f, 15.0f / 16.0f);
	tessellateBlockInWorld(block, pos, data);

	// Reset original state
	mUseFixedTexture = hadFixedTexture;

	// Potions
	const TextureUVCoordinateSet& tex = mUseFixedTexture ? mFixedTexture : blockGraphics.getTexture(BREWSTAND);

	float v0 = tex._v0;
	float v1 = tex._v1;

	for(int arm = 0; arm < 3; arm++) {
		float angle = arm * PI * 2.0f / 3.0f + PI * .5f;

		float u0 = tex.offsetWidthPixel(8);
		float u1 = tex._u1;
		if((data & (1 << arm)) != 0) {
			u1 = tex._u0;
		}

		float x0 = float(pos.x) + 8.0f / 16.0f;
		float x1 = float(pos.x) + 8.0f / 16.0f + std::sin(angle) * 8.0f / 16.0f;
		float z0 = float(pos.z) + 8.0f / 16.0f;
		float z1 = float(pos.z) + 8.0f / 16.0f + std::cos(angle) * 8.0f / 16.0f;

		// There is mild z-fighting between the center rod and the brewing stand arms, which is also present in Java
		// To eliminate this, lower vertex y position by about 0.003f
		mTessellator.vertexUV(x0, float(pos.y) + 1.0f, z0, u0, v0);
		mTessellator.vertexUV(x0, float(pos.y) + 0.0f, z0, u0, v1);
		mTessellator.vertexUV(x1, float(pos.y) + 0.0f, z1, u1, v1);
		mTessellator.vertexUV(x1, float(pos.y) + 1.0f, z1, u1, v0);

		mTessellator.vertexUV(x1, float(pos.y) + 1.0f, z1, u1, v0);
		mTessellator.vertexUV(x1, float(pos.y) + 0.0f, z1, u1, v1);
		mTessellator.vertexUV(x0, float(pos.y) + 0.0f, z0, u0, v1);
		mTessellator.vertexUV(x0, float(pos.y) + 1.0f, z0, u0, v0);
	}

	return true;
}

void BlockTessellator::tessellateTorch(const Block& block, const Vec3& pos, float xxa, float zza ){
	Vec3 temp = pos;

	const BlockGraphics& blockGraphics = *BlockGraphics::mBlocks[block.getId()];
	const TextureUVCoordinateSet& tex = mUseFixedTexture ? mFixedTexture : blockGraphics.getTexture(Facing::DOWN, block.getVariant(0));

	float u0 = tex.offsetWidth(7.f / 16.f) + TEXTURE_OFFSET;// 7
	float u1 = tex.offsetWidth(9.f / 16.f) - TEXTURE_OFFSET;//9
	float v0 = tex.offsetHeight(6.f / 16.f) + TEXTURE_OFFSET;	// 6
	float v1 = tex._v1 - TEXTURE_OFFSET;// 15.999
	float u0b = tex.offsetWidth(6.f / 16.f) + TEXTURE_OFFSET;
	float u1b = tex.offsetWidth(10.f / 16.f) - TEXTURE_OFFSET;
	float v0b = tex.offsetHeight(5.f / 16.f) + TEXTURE_OFFSET;

	float vc1 = tex.offsetHeight(2.f / 16.f, v0) - TEXTURE_OFFSET;	// 2 pixels

	temp.x += 0.5f;
	temp.z += 0.5f;

	float h = 9.9f / 16.0f;
	float r = 1 / 16.f;
	float rOffset = 1 / 16.f;

	bool isRedstone = !block.isType(Block::mTorch);
	if (!isRedstone) {
		rOffset = 0.0f;
		u0b = u0;
		u1b = u1;
		v0b = v0;
	}

	//draw top face
	mTessellator.vertexUV(temp.x + xxa * (1 - h) - r, temp.y + h, temp.z + zza * (1 - h) - r, u0, v0);
	mTessellator.vertexUV(temp.x + xxa * (1 - h) - r, temp.y + h, temp.z + zza * (1 - h) + r, u0, vc1);
	mTessellator.vertexUV(temp.x + xxa * (1 - h) + r, temp.y + h, temp.z + zza * (1 - h) + r, u1, vc1);
	mTessellator.vertexUV(temp.x + xxa * (1 - h) + r, temp.y + h, temp.z + zza * (1 - h) - r, u1, v0);

	//draw other faces - scale the offset to match h
	xxa *= h;
	zza *= h;

	if (isRedstone) {
		float h2 = h + 1.0f / 16.0f;

		temp.x += xxa*h * (h / h2) * 0.945f;	// ARRGGH. I hate this code. Used this to make everything line up correctly.
		temp.z += zza*h * (h / h2) * 0.945f;
		h = h2;
	}
	else {
		temp.x += xxa*h;
		temp.z += zza*h;
	}

	float x0 = temp.x - r;
	float x1 = temp.x + r;
	float z0 = temp.z - r;
	float z1 = temp.z + r;

	//draw bottom face
	mTessellator.vertexUV(x1 + xxa, temp.y, z1 + zza, u0, v1);
	mTessellator.vertexUV(x0 + xxa, temp.y, z1 + zza, u0, v1);
	mTessellator.vertexUV(x0 + xxa, temp.y, z0 + zza, u1, v1);
	mTessellator.vertexUV(x1 + xxa, temp.y, z0 + zza, u1, v1);

	mTessellator.vertexUV(x0, temp.y + h, z0 - rOffset, u0b, v0b);
	mTessellator.vertexUV(x0 + xxa, temp.y + 0, z0 - rOffset + zza, u0b, v1);
	mTessellator.vertexUV(x0 + xxa, temp.y + 0, z1 + rOffset + zza, u1b, v1);
	mTessellator.vertexUV(x0, temp.y + h, z1 + rOffset, u1b, v0b);

	mTessellator.vertexUV(x1, temp.y + h, z1 + rOffset, u0b, v0b);
	mTessellator.vertexUV(x1 + xxa, temp.y + 0, z1 + rOffset + zza, u0b, v1);
	mTessellator.vertexUV(x1 + xxa, temp.y + 0, z0 - rOffset + zza, u1b, v1);
	mTessellator.vertexUV(x1, temp.y + h, z0 - rOffset, u1b, v0b);

	mTessellator.vertexUV(x0 - rOffset, temp.y + h, z1, u0b, v0b);
	mTessellator.vertexUV(x0 - rOffset + xxa, temp.y + 0, z1 + zza, u0b, v1);
	mTessellator.vertexUV(x1 + rOffset + xxa, temp.y + 0, z1 + zza, u1b, v1);
	mTessellator.vertexUV(x1 + rOffset, temp.y + h, z1, u1b, v0b);

	mTessellator.vertexUV(x1 + rOffset, temp.y + h, z0, u0b, v0b);
	mTessellator.vertexUV(x1 + rOffset + xxa, temp.y + 0, z0 + zza, u0b, v1);
	mTessellator.vertexUV(x0 - rOffset + xxa, temp.y + 0, z0 + zza, u1b, v1);
	mTessellator.vertexUV(x0 - rOffset, temp.y + h, z0, u1b, v0b);
}

void BlockTessellator::tessellateCrossTexture(const Block& block, DataID data, const Vec3& pos, bool renderBothSides, float scale) {
	const BlockGraphics& blockGraphics = *BlockGraphics::mBlocks[block.getId()];
	const TextureUVCoordinateSet& tex = mUseFixedTexture ? mFixedTexture : blockGraphics.getTexture(Facing::DOWN, block.getVariant(data));
	tessellateCrossTexture(tex, pos, renderBothSides, block, scale);
}

void BlockTessellator::tessellateCrossTexture(const TextureUVCoordinateSet& tex, const Vec3& pos, bool renderBothSides, const Block& block, float scale) {
	static const float SQRT2 = 1.41421356237f;
	static const Vec3 degree45 = Vec3(SQRT2 / 2.0f, 0, SQRT2 / 2.0f);
	static const Vec3 degree135 = Vec3(-SQRT2 / 2.0f, 0, SQRT2 / 2.0f);
	static const float epsilon = 0.0001f;
	
	float polygonOffset = renderBothSides ? epsilon : 0.0f;

	Vec3 temp = pos;
	
	float u0 = tex._u0;
	float u1 = tex._u1;
	float v0 = tex._v0;
	float v1 = tex._v1;

	const float r = 0.36f;	//was 0.45f

	float x0 = temp.x + 0.5f - r;
	float x1 = temp.x + 0.5f + r;
	float z0 = temp.z + 0.5f - r;
	float z1 = temp.z + 0.5f + r;

	if (&block == Block::mTallgrass) {
		int steps = BlockPos(temp).hashCode() & 0x3;
		temp.y -= (float)steps / 16.f;
	}
	if (mUseNormals) {
		mTessellator.normal(degree45);
	}
	mTessellator.vertexUV(x0 + polygonOffset, temp.y + 1, z0, u0, v0);
	mTessellator.vertexUV(x0 + polygonOffset, temp.y + 0, z0, u0, v1);
	mTessellator.vertexUV(x1 + polygonOffset, temp.y + 0, z1, u1, v1);
	mTessellator.vertexUV(x1 + polygonOffset, temp.y + 1, z1, u1, v0);

	if (mUseNormals) {
		mTessellator.normal(degree135);
	}
	mTessellator.vertexUV(x0 + polygonOffset, temp.y + 1, z1, u0, v0);
	mTessellator.vertexUV(x0 + polygonOffset, temp.y + 0, z1, u0, v1);
	mTessellator.vertexUV(x1 + polygonOffset, temp.y + 0, z0, u1, v1);
	mTessellator.vertexUV(x1 + polygonOffset, temp.y + 1, z0, u1, v0);

	if(renderBothSides) {
		if (mUseNormals) {
			mTessellator.normal(-degree45);
		}
		mTessellator.vertexUV(x1 - polygonOffset, temp.y + 1, z1, u0, v0);
		mTessellator.vertexUV(x1 - polygonOffset, temp.y + 0, z1, u0, v1);
		mTessellator.vertexUV(x0 - polygonOffset, temp.y + 0, z0, u1, v1);
		mTessellator.vertexUV(x0 - polygonOffset, temp.y + 1, z0, u1, v0);

		if (mUseNormals) {
			mTessellator.normal(-degree135);
		}
		mTessellator.vertexUV(x1 - polygonOffset, temp.y + 1, z0, u0, v0);
		mTessellator.vertexUV(x1 - polygonOffset, temp.y + 0, z0, u0, v1);
		mTessellator.vertexUV(x0 - polygonOffset, temp.y + 0, z1, u1, v1);
		mTessellator.vertexUV(x0 - polygonOffset, temp.y + 1, z1, u1, v0);
	}
}

void BlockTessellator::tessellateCrossPolyTexture(const TextureUVCoordinateSet& tex, const Vec3& pos, bool renderBothSides, const Block& block) {
	BlockTextureTessellator::render(mTessellator, pos, tex, true, tex.pixelWidth(), 45, Vec3::ZERO, renderBothSides, mUseNormals);
	BlockTextureTessellator::render(mTessellator, pos, tex, true, tex.pixelWidth(), 135, Vec3::ZERO, renderBothSides, mUseNormals);
}

void BlockTessellator::tessellateStemTexture(const Block& block, DataID data, float h, float x, float y, float z) {
	const BlockGraphics& blockGraphics = *BlockGraphics::mBlocks[block.getId()];
	const int DISCONNECTED_STEM = 0;
	const TextureUVCoordinateSet& tex = mUseFixedTexture ? mFixedTexture : blockGraphics.getTexture(Facing::DOWN, DISCONNECTED_STEM);

	float u0 = tex._u0;
	float u1 = tex._u1;
	float v0 = tex._v0;
	float v1 = tex.offsetHeight(h) - TEXTURE_OFFSET;//tex._v1;

	float x0 = x + 0.5f - 0.45f;
	float x1 = x + 0.5f + 0.45f;
	float z0 = z + 0.5f - 0.45f;
	float z1 = z + 0.5f + 0.45f;

	mTessellator.vertexUV(x0, y + h, z0, u0, v0);
	mTessellator.vertexUV(x0, y + 0, z0, u0, v1);
	mTessellator.vertexUV(x1, y + 0, z1, u1, v1);
	mTessellator.vertexUV(x1, y + h, z1, u1, v0);

	mTessellator.vertexUV(x1, y + h, z1, u1, v0);
	mTessellator.vertexUV(x1, y + 0, z1, u1, v1);
	mTessellator.vertexUV(x0, y + 0, z0, u0, v1);
	mTessellator.vertexUV(x0, y + h, z0, u0, v0);

	mTessellator.vertexUV(x0, y + h, z1, u0, v0);
	mTessellator.vertexUV(x0, y + 0, z1, u0, v1);
	mTessellator.vertexUV(x1, y + 0, z0, u1, v1);
	mTessellator.vertexUV(x1, y + h, z0, u1, v0);

	mTessellator.vertexUV(x1, y + h, z0, u1, v0);
	mTessellator.vertexUV(x1, y + 0, z0, u1, v1);
	mTessellator.vertexUV(x0, y + 0, z1, u0, v1);
	mTessellator.vertexUV(x0, y + h, z1, u0, v0);
}

void BlockTessellator::tessellateStemDirTexture(const StemBlock& block, DataID data, int dir, float h, const Vec3& pos) {
	const BlockGraphics& blockGraphics = *BlockGraphics::mBlocks[block.getId()];
	const int CONNECTED_STEM = 1;
	const TextureUVCoordinateSet& tex = mUseFixedTexture ? mFixedTexture : blockGraphics.getTexture(Facing::DOWN, CONNECTED_STEM);
	
	float u0 = tex._u0;
	float u1 = tex._u1;
	float v0 = tex._v0;
	float v1 = tex._v1;

	float x0 = pos.x + 0.5f - 0.5f;
	float x1 = pos.x + 0.5f + 0.5f;
	float z0 = pos.z + 0.5f - 0.5f;
	float z1 = pos.z + 0.5f + 0.5f;

	float xm = pos.x + 0.5f;
	float zm = pos.z + 0.5f;

	if ((dir + 1) / 2 % 2 == 1) {
		float tmp = u1;
		u1 = u0;
		u0 = tmp;
	}

	if (dir < 2) {
		mTessellator.vertexUV(x0, pos.y + h, zm, u0, v0);
		mTessellator.vertexUV(x0, pos.y + 0, zm, u0, v1);
		mTessellator.vertexUV(x1, pos.y + 0, zm, u1, v1);
		mTessellator.vertexUV(x1, pos.y + h, zm, u1, v0);

		mTessellator.vertexUV(x1, pos.y + h, zm, u1, v0);
		mTessellator.vertexUV(x1, pos.y + 0, zm, u1, v1);
		mTessellator.vertexUV(x0, pos.y + 0, zm, u0, v1);
		mTessellator.vertexUV(x0, pos.y + h, zm, u0, v0);
	} else {
		mTessellator.vertexUV(xm, pos.y + h, z1, u0, v0);
		mTessellator.vertexUV(xm, pos.y + 0, z1, u0, v1);
		mTessellator.vertexUV(xm, pos.y + 0, z0, u1, v1);
		mTessellator.vertexUV(xm, pos.y + h, z0, u1, v0);

		mTessellator.vertexUV(xm, pos.y + h, z0, u1, v0);
		mTessellator.vertexUV(xm, pos.y + 0, z0, u1, v1);
		mTessellator.vertexUV(xm, pos.y + 0, z1, u0, v1);
		mTessellator.vertexUV(xm, pos.y + h, z1, u0, v0);
	}
}

float BlockTessellator::_findDepth(const BlockPos& p, float baseDepth) {
	float depth = 0;
	const float maxDepth = 20.f;

	for (BlockPos pp = p; pp.y > 0 && depth <= maxDepth; --pp.y) {
		auto& block = mRegion->getBlock(pp);
		const Material& material = block.getMaterial();
		if (material.isType(MaterialType::Air)) {
			depth += 1.f;
		} else if (!material.isType(MaterialType::Water)) {
			break;
		}
	}

	return std::min(1.f, (depth / maxDepth) + baseDepth);
}

#define _IN(X, Z)   in[(Z) + (X) * 3]
#define _OUT(X, Z)   out[(Z) + (X) * 2]

template<typename T>
void _gather3x3(T* in, T* out) {
	for (int x = 0; x <= 1; ++x) {
		for (int z = 0; z <= 1; ++z) {
			auto& co = _OUT(x, z);

			co = (
				_IN(x, z) +
				_IN(x + 1, z) +
				_IN(x + 1, z + 1) +
				_IN(x, z + 1)
				) / 4;
		}
	}
}

BrightnessPair BlockTessellator::_getLightColorForWater(const BlockPos& pos, BrightnessPair insideBlock) {
	const Block& block = mBlockCache.getBlock(pos);
	if ( Block::mTranslucency[block.mID] > 0.5f ) {
		return getLightColor(pos);
	} else {
		return insideBlock;	//use this fixed occlusion value for blocking blocks, it kind of works
	}
}

bool BlockTessellator::tessellateLiquidInWorld(const LiquidBlock& block, const BlockPos& p, DataID data){
	BlockOccluder occluder(mBlockCache, block, p, mCurrentShape, Facing::ALL_FACES, mRenderingGUI);
	
	const Material& m = block.getMaterial();

	float h0 = getWaterHeight(p, m);
	float h1 = getWaterHeight(p.south(), m);
	float h2 = getWaterHeight({ p.x + 1, p.y, p.z + 1 }, m);
	float h3 = getWaterHeight(p.east(), m);

	bool renderUpFaceAnyway = h0 < 0.85f || h1 < 0.85f || h2 < 0.85f || h3 < 0.85f;

	if (!occluder.anyVisible() && !renderUpFaceAnyway) {
		return false;	//nothing to be done
	}
	
	bool changed = false;

	//don't apply shading on emitting blocks!
	//NOTE: NOT LIKE PC!
	bool e = Block::mLightEmission[block.mID] > 0;
	const float c11 = 1;
	const float c10 = e ? 0.875f : 0.5f;
	const float c2 = e ? 0.95f : 0.8f;
	const float c3 = e ? 0.9f : 0.6f;

	float offs = 0.001f;
	Color vcolors[4] = {
		Color::WHITE, Color::WHITE, Color::WHITE, Color::WHITE
	};

	if (block.getMaterial().isType(MaterialType::Water)) {
		//read all neighboring colors
		bool useSimpleDepthBlend = true;
		Color in[9];
		Color* lastColor = nullptr;

		for (int x = -1; x <= 1; ++x) {
			for (int z = -1; z <= 1; ++z) {
				auto& color = mRegion->getBiome(p + BlockPos(x, 0, z)).mWaterColor;
				if (&color != lastColor && lastColor) {	//check if there are two different biomes affecting this block
					useSimpleDepthBlend = false;
				}

				lastColor = &color;

				_IN(x + 1, z + 1) = color;
			}
		}

		//get depth for this block, no need to smooth here
		if (useSimpleDepthBlend) {
			float depth = _findDepth(p, in[4].a);

			for (auto& c : in) {
				c.a = depth;
			}
		}
		else {
			for (int x = -1; x <= 1; ++x) {
				for (int z = -1; z <= 1; ++z) {
					_IN(x + 1, z + 1).a = _findDepth(p + BlockPos(x, 0, z), _IN(x + 1, z + 1).a);
				}
			}
		}

		_gather3x3(in, vcolors);
	}

	const BlockGraphics& blockGraphics = *BlockGraphics::mBlocks[block.getId()];
	if (!occluder.occludes(Facing::UP) || renderUpFaceAnyway) {
		changed = true;
		// First try with the still water
		TextureUVCoordinateSet tex = blockGraphics.getTexture(Facing::UP);
		float angle = (float)LiquidBlock::getSlopeAngle(*mRegion, p, m);
		if (angle > -999) {
			// If it is flowing, go with the flowing one
			// These use the same hacks as the PC version where
			// face up and down is still and other sides are flowing
			tex = blockGraphics.getTexture(Facing::NORTH);
		}

		h0 -= offs;
		h1 -= offs;
		h2 -= offs;
		h3 -= offs;

		float u00, u01, u10, u11;
		float v00, v01, v10, v11;

		if (angle < -999) {
			//u00 = tex._u0;
			//v00 = tex._v0;
			u00 = 0;
			v00 = 0;
			u01 = u00;
			//v01 = tex._v1;	// tex.getV(SharedConstants.WORLD_RESOLUTION);
			//u10 = tex._u1;	// tex.getU(SharedConstants.WORLD_RESOLUTION);
			v01 = 1;	// tex.getV(SharedConstants.WORLD_RESOLUTION);
			u10 = 1;	// tex.getU(SharedConstants.WORLD_RESOLUTION);
			v10 = v01;
			u11 = u10;
			v11 = v00;
		}
		else {
			const float s = Math::sin(angle) * .5f;
			const float c = Math::cos(angle) * .5f;

			//u00 = tex.offsetWidth(1 - c - s);//tex.getU(cc + (-c - s) * SharedConstants.WORLD_RESOLUTION);
			//v00 = tex.offsetHeight(1 - c + s);	//tex.getV(cc + (-c + s) * SharedConstants.WORLD_RESOLUTION);
			//u01 = tex.offsetWidth(1 - c + s);//tex.getU(cc + (-c + s) * SharedConstants.WORLD_RESOLUTION);
			//v01 = tex.offsetHeight(1 + c + s);	//tex.getV(cc + (+c + s) * SharedConstants.WORLD_RESOLUTION);
			//u10 = tex.offsetWidth(1 + c + s);//tex.getU(cc + (+c + s) * SharedConstants.WORLD_RESOLUTION);
			//v10 = tex.offsetHeight(1 + c - s);	//tex.getV(cc + (+c - s) * SharedConstants.WORLD_RESOLUTION);
			//u11 = tex.offsetWidth(1 + c - s);//tex.getU(cc + (+c - s) * SharedConstants.WORLD_RESOLUTION);
			//v11 = tex.offsetHeight(1 - c - s);	//tex.getV(cc + (-c - s) * SharedConstants.WORLD_RESOLUTION);

			u10 = (1 - c - s);//tex.getU(cc + (-c - s) * SharedConstants.WORLD_RESOLUTION);
			v10 = (1 - c + s);	//tex.getV(cc + (-c + s) * SharedConstants.WORLD_RESOLUTION);
			u11 = (1 - c + s);//tex.getU(cc + (-c + s) * SharedConstants.WORLD_RESOLUTION);
			v11 = (1 + c + s);	//tex.getV(cc + (+c + s) * SharedConstants.WORLD_RESOLUTION);
			u00 = (1 + c + s);//tex.getU(cc + (+c + s) * SharedConstants.WORLD_RESOLUTION);
			v00 = (1 + c - s);	//tex.getV(cc + (+c - s) * SharedConstants.WORLD_RESOLUTION);
			u01 = (1 + c - s);//tex.getU(cc + (+c - s) * SharedConstants.WORLD_RESOLUTION);
			v01 = (1 - c - s);	//tex.getV(cc + (-c - s) * SharedConstants.WORLD_RESOLUTION);
		}

		auto thisblock = _getLightColorForWater(p, { Brightness(11), Brightness(11) });

		Vec2 br[4];

		if (mSmoothLighting) {
			Vec2 in[9];
			for (int x = -1; x <= 1; ++x) {
				for (int z = -1; z <= 1; ++z) {
					_IN(x + 1, z + 1) = toUV(_getLightColorForWater(p + BlockPos(x, 1, z), thisblock));
				}
			}
			_gather3x3(in, br);
		}
		else {
			br[0] = br[1] = br[2] = br[3] = toUV(_getLightColorForWater(p.above(), thisblock));
		}

		_tex1(br[0]);
		mTessellator.color(vcolors[0]);
		//mTessellator.vertexUV(p.x + 0.0f, p.y + h0, p.z + 0.0f, u00, v00);
		mTessellator.vertexUV(p.x + 0.0f, p.y + h0, p.z + 0.0f, u01, v01);

		_tex1(br[1]);
		mTessellator.color(vcolors[1]);
		//mTessellator.vertexUV(p.x + 0.0f, p.y + h1, p.z + 1.0f, u01, v01);
		mTessellator.vertexUV(p.x + 0.0f, p.y + h1, p.z + 1.0f, u00, v00);

		_tex1(br[3]);
		mTessellator.color(vcolors[3]);
		//mTessellator.vertexUV(p.x + 1.0f, p.y + h2, p.z + 1.0f, u10, v10);
		mTessellator.vertexUV(p.x + 1.0f, p.y + h2, p.z + 1.0f, u11, v11);

		_tex1(br[2]);
		mTessellator.color(vcolors[2]);
		//mTessellator.vertexUV(p.x + 1.0f, p.y + h3, p.z + 0.0f, u11, v11);
		mTessellator.vertexUV(p.x + 1.0f, p.y + h3, p.z + 0.0f, u10, v10);
	}

	if(!occluder.occludes(Facing::DOWN)) {

		_tex1(toUV(getLightColor(p.below())));
		mTessellator.color(vcolors[0] * c10);
		renderFaceDown(block, p, blockGraphics.getTexture(Facing::DOWN));
		changed = true;
	}

	for (FacingID face = 2; face < 6; face++) {
		TextureUVCoordinateSet tex = blockGraphics.getTexture(face);

		if(!occluder.occludes(face)) {
			float hh0;
			float hh1;
			float x0, z0, x1, z1;

			switch (face) {
			case Facing::NORTH:
				hh0 = h0;
				hh1 = h3;
				x0 = float(p.x);
				x1 = float(p.x + 1);
				z0 = float(p.z + offs);
				z1 = float(p.z + offs);
				break;

			case Facing::SOUTH:
				hh0 = h2;
				hh1 = h1;
				x0 = float(p.x + 1);
				x1 = float(p.x);
				z0 = float(p.z + 1 - offs);
				z1 = float(p.z + 1 - offs);
				break;

			case Facing::WEST:
				hh0 = h1;
				hh1 = h0;
				x0 = float(p.x + offs);
				x1 = float(p.x + offs);
				z0 = float(p.z + 1);
				z1 = float(p.z);
				break;

			case Facing::EAST:
				hh0 = h3;
				hh1 = h2;
				x0 = float(p.x + 1 - offs);
				x1 = float(p.x + 1 - offs);
				z0 = float(p.z);
				z1 = float(p.z + 1);
				break;

			default:
				hh0 = hh1 = x0 = x1 = z0 = z1 = 0.f;		// Silence uninitialized variable warnings.
				DEBUG_FAIL("Invalid enum");
				break;
			}

			changed = true;
			//float u0 = tex._u0;
			//float u1 = tex._u1;

			//float v01 = tex.offsetHeight((1 - hh0));
			//float v02 = tex.offsetHeight((1 - hh1));
			//float v1 = tex.offsetHeight(1);

			BlockPos n = p.neighbor(face);
			float fbr = face < 2 ? c2 : c3;

			_tex1(toUV(getLightColor(n)));

			float vo = 0.f;

			mTessellator.color(vcolors[0] * c11 * fbr);
			//mTessellator.vertexUV(x0, p.y + hh0, z0, u0, v01);
			//mTessellator.vertexUV(x1, p.y + hh1, z1, u1, v02);
			//mTessellator.vertexUV(x1, p.y + vo, z1, u1, v1);
			//mTessellator.vertexUV(x0, p.y + vo, z0, u0, v1);
			mTessellator.vertexUV(x0, p.y + hh0, z0, 0, 0);
			mTessellator.vertexUV(x1, p.y + hh1, z1, 1, 0);
			mTessellator.vertexUV(x1, p.y + vo, z1, 1, 1);
			mTessellator.vertexUV(x0, p.y + vo, z0, 0, 1);
		}
	}

	return changed;
}

float BlockTessellator::getWaterHeight( const BlockPos& p, const Material& m ){
	int count = 0;
	float h = 0;

	for (int i = 0; i < 4; i++) {
		BlockPos n(
			p.x - (i & 1),
			p.y,
			p.z - ((i >> 1) & 1));

		if (mRegion->getMaterial(n.above()) == m) {
			return 1;
		}

		const Material& tm = mRegion->getMaterial(n);
		if (tm == m) {
			int d = getData(n);
			if (d >= 8 || d == 0) {
				h += (LiquidBlock::getHeightFromData(d)) * 10;
				count += 10;
			}
			h += LiquidBlock::getHeightFromData(d);
			count++;
		} else if (!tm.isSolid()) {
			h += 1;
			count++;
		}
	}

	return 1 - h / count;
}

bool BlockTessellator::tessellateBlockInWorldWithAmbienceOcclusion(const Block& block, BlockPos p, int data, const Color& base, BlockOccluder* occluder){
	
	const float Epsilon = 0.0005f;

	mApplyAmbientOcclusion = true;

	bool i = false;
	bool tintSides = true;
	bool tintBottom = true;

	if (&block == Block::mGrass) {
		tintSides = false;
		tintBottom = false;
	}

	AmbientOcclusionCalculator aoc(p, block, mBlockCache);
	bool translucent = aoc.isTranslucent(p);
	aoc.setSeasons(block.isSeasonTinted(*mRegion, p));
	aoc.setBaseColor(base);
	aoc.setOutputColors(mAoColors);
	aoc.setOutputLightTexturePositions(mTc);

	// Down
	if(!occluder || !occluder->occludes(Facing::DOWN)) {
		bool touchEdge = mCurrentShape.min.y <= Epsilon;

		aoc.setTintSides(tintBottom);
		aoc.setTouchEdge(touchEdge);
		aoc.calculate(Facing::DOWN, translucent);
		renderFaceDown(block, p, _getTexture(block, Facing::DOWN, data));
		i = true;
	}

	if(!occluder || !occluder->occludes(Facing::UP)) {
		bool touchEdge = mCurrentShape.max.y >= 1 - Epsilon;

		aoc.setTintSides(true);
		aoc.setTouchEdge(touchEdge);
		aoc.calculate(Facing::UP, translucent);
		renderFaceUp(block, p, _getTexture(block, Facing::UP, data));
		i = true;
	}

	if(!occluder || !occluder->occludes(Facing::NORTH)) {
		bool touchEdge = mCurrentShape.min.z <= Epsilon;

		aoc.setTintSides(tintSides);
		aoc.setTouchEdge(touchEdge);
		aoc.calculate(Facing::NORTH, translucent);
		renderNorth(block, p, _getTexture(block, Facing::NORTH, data));
		i = true;
	}

	if(!occluder || !occluder->occludes(Facing::SOUTH)) {
		bool touchEdge = mCurrentShape.max.z >= 1 - Epsilon;

		aoc.setTintSides(tintSides);
		aoc.setTouchEdge(touchEdge);
		aoc.calculate(Facing::SOUTH, translucent);
		renderSouth(block, p, _getTexture(block, Facing::SOUTH, data));
		i = true;
	}

	if(!occluder || !occluder->occludes(Facing::WEST)) {
		bool touchEdge = mCurrentShape.min.x <= Epsilon;

		aoc.setTintSides(tintSides);
		aoc.setTouchEdge(touchEdge);
		aoc.calculate(Facing::WEST, translucent);
		renderWest(block, p, _getTexture(block, Facing::WEST, data));
		i = true;
	}

	if(!occluder || !occluder->occludes(Facing::EAST)) {
		bool touchEdge = mCurrentShape.max.x >= 1 - Epsilon;

		aoc.setTintSides(tintSides);
		aoc.setTouchEdge(touchEdge);
		aoc.calculate(Facing::EAST, translucent);
		renderEast(block, p, _getTexture(block, Facing::EAST, data));
		i = true;
	}

	mApplyAmbientOcclusion = false;

	mFlipFace[0] = mFlipFace[1] = mFlipFace[2] = mFlipFace[3] = mFlipFace[4] = mFlipFace[5] = 0;

	return i;
}

bool BlockTessellator::tessellateCactusInWorld(const Block& block, const BlockPos& p, int data) {
	const Color& base = Color::fromARGB(block.getColor(data));
	const float s = 1 / 16.0f;

	BlockOccluder occluder(mBlockCache, block, p, mCurrentShape, { Facing::DOWN, Facing::UP }, mRenderingGUI);

	auto centerLightColor = getLightColor(p);

	if(!occluder.occludes(Facing::DOWN)) {
		_tex1(toUV(getLightColor(p.below())));
		mTessellator.color(base * Color::SHADE_DOWN);
		renderFaceDown(block, p, _getTexture(block, 0, data));
	}

	if(!occluder.occludes(Facing::UP)) {
		//TODO: Check code below, looks weird
		//if (mCurrentShape.max.y != 1 && !block.material->isLiquid()) br = centerBrightness;
		_tex1(toUV(mCurrentShape.max.y < 1 ? centerLightColor : getLightColor(p.above())));
		mTessellator.color(base * Color::SHADE_UP);
		renderFaceUp(block, p, _getTexture(block, 1, data));
	}

	// We decided not to check the sides for occlusion, if we want at some point in the future,
	// we can check all sides if they are occluded and then not render them. The plan is though
	// to move that check outside BlockTessellator

	// Using the center shading for all sides
	_tex1(toUV(centerLightColor));

	// Same for north and south
	mTessellator.color(base * Color::SHADE_NORTH_SOUTH);

	// North
	mTessellator.addOffset(0, 0, s);
	renderNorth(block, p, _getTexture(block, Facing::NORTH, data));
	mTessellator.addOffset(0, 0, -s);

	// South
	mTessellator.addOffset(0, 0, -s);
	renderSouth(block, p, _getTexture(block, Facing::SOUTH, data));
	mTessellator.addOffset(0, 0, s);

	// Same for East and West
	mTessellator.color(base * Color::SHADE_WEST_EAST);

	mTessellator.addOffset(s, 0, 0);
	renderWest(block, p, _getTexture(block, Facing::WEST, data));
	mTessellator.addOffset(-s, 0, 0);

	mTessellator.addOffset(-s, 0, 0);
	renderEast(block, p, _getTexture(block, Facing::EAST, data));
	mTessellator.addOffset(s, 0, 0);

	return true;
}

bool BlockTessellator::tessellateFenceInWorld(const FenceBlock& block, const BlockPos& p, int data) {
	bool changed = true;
	AABB tmpAABB;

	float a = 6 / 16.0f;
	float b = 10 / 16.0f;
	float h = 1.f - TEXTURE_OFFSET;
	mCurrentShape.set(a, 0, a, b, h, b);
	tessellateBlockInWorld(block, p, data);

	bool vertical = false;
	bool horizontal = false;

	bool l = block.connectsTo(*mRegion, p, p.west());
	bool r = block.connectsTo(*mRegion, p, p.east());
	bool u = block.connectsTo(*mRegion, p, p.north());
	bool d = block.connectsTo(*mRegion, p, p.south());

	if (l || r) {
		vertical = true;
	}

	if (u || d) {
		horizontal = true;
	}

	if (!vertical && !horizontal) {
		vertical = true;
	}

	a = 7 / 16.0f;
	b = 9 / 16.0f;
	float h0 = 12 / 16.0f;
	float h1 = 15 / 16.0f;

	float x0 = l ? 0 : a;
	float x1 = r ? 1 : b;
	float z0 = u ? 0 : a;
	float z1 = d ? 1 : b;

	if (vertical) {
		mCurrentShape.set(x0, h0, a, x1, h1, b);
		tessellateBlockInWorld(block, p, data);
	}

	if (horizontal) {
		mCurrentShape.set(a, h0, z0, b, h1, z1);
		tessellateBlockInWorld(block, p, data);
	}

	h0 = 6 / 16.0f;
	h1 = 9 / 16.0f;
	if (vertical) {
		mCurrentShape.set(x0, h0, a, x1, h1, b);
		tessellateBlockInWorld(block, p, data);
	}

	if (horizontal) {
		mCurrentShape.set(a, h0, z0, b, h1, z1);
		tessellateBlockInWorld(block, p, data);
	}

	return changed;
}

bool BlockTessellator::tessellateFenceGateInWorld(const FenceGateBlock& block, const BlockPos& p, DataID data) {
	bool changed = true;

	bool isOpen = Block::mFenceGateOak->getBlockState(BlockState::OpenBit).getBool(data);
	int direction = Block::mFenceGateOak->getBlockState(BlockState::Direction).get<int>(data);
	bool isInWall = Block::mFenceGateOak->getBlockState(BlockState::InWallBit).getBool(data);

	const float h00 = 6 / 16.0f;
	const float h01 = 9 / 16.0f;
	const float h10 = 12 / 16.0f;
	const float h11 = 15 / 16.0f;
	const float h20 = 5 / 16.0f;
	const float h21 = (16 / 16.0f) - TEXTURE_OFFSET;

	Vec3 offset(0.0f, isInWall ? -3.0f / 16.0f : 0.0f, 0.0f);
	mTessellator.addOffset(offset);
	// edge sticks
	if (direction == Direction::EAST || direction == Direction::WEST) {
		float x0 = 7 / 16.0f;
		float x1 = 9 / 16.0f;
		float z0 = 0 / 16.0f;
		float z1 = 2 / 16.0f;
		mCurrentShape.set(x0, h20, z0, x1, h21, z1);
		tessellateBlockInWorld(block, p, data);

		z0 = 14 / 16.0f;
		z1 = 16 / 16.0f;
		mCurrentShape.set(x0, h20, z0, x1, h21, z1);
		tessellateBlockInWorld(block, p, data);
	} else {
		float x0 = 0 / 16.0f;
		float x1 = 2 / 16.0f;
		float z0 = 7 / 16.0f;
		float z1 = 9 / 16.0f;
		mCurrentShape.set(x0, h20, z0, x1, h21, z1);
		tessellateBlockInWorld(block, p, data);

		x0 = 14 / 16.0f;
		x1 = 16 / 16.0f;
		mCurrentShape.set(x0, h20, z0, x1, h21, z1);
		tessellateBlockInWorld(block, p, data);
	}

	if (!isOpen) {
		if (direction == Direction::EAST || direction == Direction::WEST) {
			float x0 = 7 / 16.0f;
			float x1 = 9 / 16.0f;
			float z0 = 6 / 16.0f;
			float z1 = 8 / 16.0f;
			mCurrentShape.set(x0, h00, z0, x1, h11, z1);
			tessellateBlockInWorld(block, p, data);
			z0 = 8 / 16.0f;
			z1 = 10 / 16.0f;
			mCurrentShape.set(x0, h00, z0, x1, h11, z1);
			tessellateBlockInWorld(block, p, data);
			z0 = 10 / 16.0f;
			z1 = 14 / 16.0f;
			mCurrentShape.set(x0, h00, z0, x1, h01, z1);
			tessellateBlockInWorld(block, p, data);
			mCurrentShape.set(x0, h10, z0, x1, h11, z1);
			tessellateBlockInWorld(block, p, data);
			z0 = 2 / 16.0f;
			z1 = 6 / 16.0f;
			mCurrentShape.set(x0, h00, z0, x1, h01, z1);
			tessellateBlockInWorld(block, p, data);
			mCurrentShape.set(x0, h10, z0, x1, h11, z1);
			tessellateBlockInWorld(block, p, data);
		} else {
			float x0 = 6 / 16.0f;
			float x1 = 8 / 16.0f;
			float z0 = 7 / 16.0f;
			float z1 = 9 / 16.0f;
			mCurrentShape.set(x0, h00, z0, x1, h11, z1);
			tessellateBlockInWorld(block, p, data);
			x0 = 8 / 16.0f;
			x1 = 10 / 16.0f;
			mCurrentShape.set(x0, h00, z0, x1, h11, z1);
			tessellateBlockInWorld(block, p, data);
			x0 = 10 / 16.0f;
			x1 = 14 / 16.0f;
			mCurrentShape.set(x0, h00, z0, x1, h01, z1);
			tessellateBlockInWorld(block, p, data);
			mCurrentShape.set(x0, h10, z0, x1, h11, z1);
			tessellateBlockInWorld(block, p, data);
			x0 = 2 / 16.0f;
			x1 = 6 / 16.0f;
			mCurrentShape.set(x0, h00, z0, x1, h01, z1);
			tessellateBlockInWorld(block, p, data);
			mCurrentShape.set(x0, h10, z0, x1, h11, z1);
			tessellateBlockInWorld(block, p, data);

		}
	} else {
		if (direction == Direction::EAST) {

			const float z00 = 0 / 16.0f;
			const float z01 = 2 / 16.0f;
			const float z10 = 14 / 16.0f;
			const float z11 = 16 / 16.0f;

			const float x0 = 9 / 16.0f;
			const float x1 = 13 / 16.0f;
			const float x2 = 15 / 16.0f;

			mCurrentShape.set(x1, h00, z00, x2, h11, z01);
			tessellateBlockInWorld(block, p, data);
			mCurrentShape.set(x1, h00, z10, x2, h11, z11);
			tessellateBlockInWorld(block, p, data);

			mCurrentShape.set(x0, h00, z00, x1, h01, z01);
			tessellateBlockInWorld(block, p, data);
			mCurrentShape.set(x0, h00, z10, x1, h01, z11);
			tessellateBlockInWorld(block, p, data);

			mCurrentShape.set(x0, h10, z00, x1, h11, z01);
			tessellateBlockInWorld(block, p, data);
			mCurrentShape.set(x0, h10, z10, x1, h11, z11);
			tessellateBlockInWorld(block, p, data);
		} else if (direction == Direction::WEST) {
			const float z00 = 0 / 16.0f;
			const float z01 = 2 / 16.0f;
			const float z10 = 14 / 16.0f;
			const float z11 = 16 / 16.0f;

			const float x0 = 1 / 16.0f;
			const float x1 = 3 / 16.0f;
			const float x2 = 7 / 16.0f;

			mCurrentShape.set(x0, h00, z00, x1, h11, z01);
			tessellateBlockInWorld(block, p, data);
			mCurrentShape.set(x0, h00, z10, x1, h11, z11);
			tessellateBlockInWorld(block, p, data);

			mCurrentShape.set(x1, h00, z00, x2, h01, z01);
			tessellateBlockInWorld(block, p, data);
			mCurrentShape.set(x1, h00, z10, x2, h01, z11);
			tessellateBlockInWorld(block, p, data);

			mCurrentShape.set(x1, h10, z00, x2, h11, z01);
			tessellateBlockInWorld(block, p, data);
			mCurrentShape.set(x1, h10, z10, x2, h11, z11);
			tessellateBlockInWorld(block, p, data);
		} else if (direction == Direction::SOUTH) {

			const float x00 = 0 / 16.0f;
			const float x01 = 2 / 16.0f;
			const float x10 = 14 / 16.0f;
			const float x11 = 16 / 16.0f;

			const float z0 = 9 / 16.0f;
			const float z1 = 13 / 16.0f;
			const float z2 = 15 / 16.0f;

			mCurrentShape.set(x00, h00, z1, x01, h11, z2);
			tessellateBlockInWorld(block, p, data);
			mCurrentShape.set(x10, h00, z1, x11, h11, z2);
			tessellateBlockInWorld(block, p, data);

			mCurrentShape.set(x00, h00, z0, x01, h01, z1);
			tessellateBlockInWorld(block, p, data);
			mCurrentShape.set(x10, h00, z0, x11, h01, z1);
			tessellateBlockInWorld(block, p, data);

			mCurrentShape.set(x00, h10, z0, x01, h11, z1);
			tessellateBlockInWorld(block, p, data);
			mCurrentShape.set(x10, h10, z0, x11, h11, z1);
			tessellateBlockInWorld(block, p, data);
		} else if (direction == Direction::NORTH) {
			const float x00 = 0 / 16.0f;
			const float x01 = 2 / 16.0f;
			const float x10 = 14 / 16.0f;
			const float x11 = 16 / 16.0f;

			const float z0 = 1 / 16.0f;
			const float z1 = 3 / 16.0f;
			const float z2 = 7 / 16.0f;

			mCurrentShape.set(x00, h00, z0, x01, h11, z1);
			tessellateBlockInWorld(block, p, data);
			mCurrentShape.set(x10, h00, z0, x11, h11, z1);
			tessellateBlockInWorld(block, p, data);

			mCurrentShape.set(x00, h00, z1, x01, h01, z2);
			tessellateBlockInWorld(block, p, data);
			mCurrentShape.set(x10, h00, z1, x11, h01, z2);
			tessellateBlockInWorld(block, p, data);

			mCurrentShape.set(x00, h10, z1, x01, h11, z2);
			tessellateBlockInWorld(block, p, data);
			mCurrentShape.set(x10, h10, z1, x11, h11, z2);
			tessellateBlockInWorld(block, p, data);
		}
	}

	mTessellator.addOffset(-offset);

	return changed;
}

bool BlockTessellator::tessellateBedInWorld(const Block& block, const BlockPos& p, DataID data) {
	int direction = Block::mBed->getBlockState(BlockState::Direction).get<int>(data);
	bool isHead = BedBlock::isHeadPiece(data);

	auto centerColor = getLightColor(p);
	// render wooden underside
	{
		_tex1(toUV(centerColor));
		mTessellator.color(Color::SHADE_DOWN);
		const TextureUVCoordinateSet& tex = mUseFixedTexture ? mFixedTexture : _getTexture(block, Facing::DOWN, data);

		float u0 = tex._u0;
		float u1 = tex._u1;
		float v0 = tex._v0;
		float v1 = tex._v1;

		float x0 = p.x + mCurrentShape.min.x;
		float x1 = p.x + mCurrentShape.max.x;
		float y0 = p.y + mCurrentShape.min.y + 3.0f / 16.0f;
		float z0 = p.z + mCurrentShape.min.z;
		float z1 = p.z + mCurrentShape.max.z;

		mTessellator.vertexUV(x0, y0, z1, u0, v1);
		mTessellator.vertexUV(x0, y0, z0, u0, v0);
		mTessellator.vertexUV(x1, y0, z0, u1, v0);
		mTessellator.vertexUV(x1, y0, z1, u1, v1);
	}

	// render bed top
	mTessellator.color(Color::SHADE_UP);

	const TextureUVCoordinateSet& tex = mUseFixedTexture ? mFixedTexture : _getTexture(block, Facing::UP, data);

	float u0 = tex._u0;
	float u1 = tex._u1;
	float v0 = tex._v0;
	float v1 = tex._v1;

	// Default is west
	float topLeftU = u0;
	float topRightU = u1;
	float topLeftV = v0;
	float topRightV = v0;
	float bottomLeftU = u0;
	float bottomRightU = u1;
	float bottomLeftV = v1;
	float bottomRightV = v1;

	if (direction == Direction::SOUTH) {
		// rotate 90 degrees clockwise
		topRightU = u0;
		topLeftV = v1;
		bottomLeftU = u1;
		bottomRightV = v0;
	} else if (direction == Direction::NORTH) {
		// rotate 90 degrees counter-clockwise
		topLeftU = u1;
		topRightV = v1;
		bottomRightU = u0;
		bottomLeftV = v0;
	} else if (direction == Direction::EAST) {
		// rotate 180 degrees
		topLeftU = u1;
		topRightV = v1;
		bottomRightU = u0;
		bottomLeftV = v0;
		topRightU = u0;
		topLeftV = v1;
		bottomLeftU = u1;
		bottomRightV = v0;
	}

	float x0 = p.x + mCurrentShape.min.x;
	float x1 = p.x + mCurrentShape.max.x;
	float y1 = p.y + mCurrentShape.max.y;
	float z0 = p.z + mCurrentShape.min.z;
	float z1 = p.z + mCurrentShape.max.z;

	mTessellator.vertexUV(x1, y1, z1, bottomLeftU, bottomLeftV);
	mTessellator.vertexUV(x1, y1, z0, topLeftU, topLeftV);
	mTessellator.vertexUV(x0, y1, z0, topRightU, topRightV);
	mTessellator.vertexUV(x0, y1, z1, bottomRightU, bottomRightV);

	// determine which edge to skip (the one between foot and head piece)
	auto skipEdge = Direction::DIRECTION_FACING[direction];
	if (isHead) {
		skipEdge = Direction::DIRECTION_FACING[Direction::DIRECTION_OPPOSITE[direction]];
	}

	// and which edge to x-flip
	auto flipEdge = Facing::WEST;

	switch (direction) {
	case Direction::NORTH:
		break;
	case Direction::SOUTH:
		flipEdge = Facing::EAST;
		break;
	case Direction::EAST:
		flipEdge = Facing::NORTH;
		break;
	case Direction::WEST:
		flipEdge = Facing::SOUTH;
		break;
	}

	if (!mUseFixedTexture) {
		BlockOccluder occluder(mBlockCache, block, p, mCurrentShape, Facing::ALL_EXCEPT_AXIS_Y[skipEdge], mRenderingGUI);

		if (!occluder.occludes(Facing::NORTH)) {
			_tex1(toUV(mCurrentShape.min.z > 0 ? centerColor : getLightColor(p.north())));
			mTessellator.color(Color::SHADE_NORTH_SOUTH);
			BlockTextureTessellator::render(mTessellator, Vec3((float)p.x, (float)p.y, (float)p.z - 0.5f), _getTexture(block, 2, data), true, tex.pixelWidth(), flipEdge == Facing::NORTH ? 180.0f : 0.0f);
		}

		if (!occluder.occludes(Facing::SOUTH)) {
			_tex1(toUV(mCurrentShape.max.z < 1 ? centerColor : getLightColor(p.south())));
			mTessellator.color(Color::SHADE_NORTH_SOUTH);
			BlockTextureTessellator::render(mTessellator, Vec3((float)p.x, (float)p.y, (float)p.z + 0.5f), _getTexture(block, 3, data), true, tex.pixelWidth(), flipEdge == Facing::SOUTH ? 0.0f : 180.0f);
		}

		if (!occluder.occludes(Facing::WEST)) {
			_tex1(toUV(mCurrentShape.min.x > 0 ? centerColor : getLightColor(p.west())));
			mTessellator.color(Color::SHADE_WEST_EAST);
			BlockTextureTessellator::render(mTessellator, Vec3((float)p.x - 0.5f, (float)p.y, (float)p.z), _getTexture(block, 4, data), true, tex.pixelWidth(), flipEdge == Facing::WEST ? 270.0f : 90.0f);
		}

		if (!occluder.occludes(Facing::EAST)) {
			_tex1(toUV(mCurrentShape.max.x < 1 ? centerColor : getLightColor(p.east())));
			mTessellator.color(Color::SHADE_WEST_EAST);
			BlockTextureTessellator::render(mTessellator, Vec3((float)p.x + 0.5f, (float)p.y, (float)p.z), _getTexture(block, 5, data), true, tex.pixelWidth(), flipEdge == Facing::EAST ? 90.0f : 270.0f);
		}
	}

	mXFlipTexture = false;
	return true;
}

bool BlockTessellator::tessellateVineInWorld(const Block& block, const BlockPos& p, DataID data) {
	const BlockGraphics& blockGraphics = *BlockGraphics::mBlocks[block.getId()];
	auto tex = blockGraphics.getTexture(Facing::DOWN);

	if(mUseFixedTexture) {
		tex = mFixedTexture;
	}

	Color blockColor = Color::fromARGB(block.getColor(*mRegion, p, data));

	mTessellator.color(blockColor);
	_tex1(toUV(getLightColor(p)));

	float u0 = tex._u0;
	float v0 = tex._v0;
	float u1 = tex._u1;
	float v1 = tex._v1;

	float r = 0.05f;
	int facings = data;

	if((facings& VineBlock::VINE_WEST) != 0) {
		mTessellator.vertexUV(p.x + r, p.y + 1.0f, p.z + 1.0f, u0, v0);
		mTessellator.vertexUV(p.x + r, p.y + 0.0f, p.z + 1.0f, u0, v1);
		mTessellator.vertexUV(p.x + r, p.y + 0.0f, p.z + 0.0f, u1, v1);
		mTessellator.vertexUV(p.x + r, p.y + 1.0f, p.z + 0.0f, u1, v0);

		mTessellator.vertexUV(p.x + r, p.y + 1.0f, p.z + 0.0f, u1, v0);
		mTessellator.vertexUV(p.x + r, p.y + 0.0f, p.z + 0.0f, u1, v1);
		mTessellator.vertexUV(p.x + r, p.y + 0.0f, p.z + 1.0f, u0, v1);
		mTessellator.vertexUV(p.x + r, p.y + 1.0f, p.z + 1.0f, u0, v0);
	}

	if((facings& VineBlock::VINE_EAST) != 0) {
		mTessellator.vertexUV(p.x + 1.0f - r, p.y + 0.0f, p.z + 1.0f, u1, v1);
		mTessellator.vertexUV(p.x + 1.0f - r, p.y + 1.0f, p.z + 1.0f, u1, v0);
		mTessellator.vertexUV(p.x + 1.0f - r, p.y + 1.0f, p.z + 0.0f, u0, v0);
		mTessellator.vertexUV(p.x + 1.0f - r, p.y + 0.0f, p.z + 0.0f, u0, v1);

		mTessellator.vertexUV(p.x + 1.0f - r, p.y + 0.0f, p.z + 0.0f, u0, v1);
		mTessellator.vertexUV(p.x + 1.0f - r, p.y + 1.0f, p.z + 0.0f, u0, v0);
		mTessellator.vertexUV(p.x + 1.0f - r, p.y + 1.0f, p.z + 1.0f, u1, v0);
		mTessellator.vertexUV(p.x + 1.0f - r, p.y + 0.0f, p.z + 1.0f, u1, v1);
	}

	if((facings& VineBlock::VINE_NORTH) != 0) {
		mTessellator.vertexUV(p.x + 1.0f, p.y + 0.0f, p.z + r, u1, v1);
		mTessellator.vertexUV(p.x + 1.0f, p.y + 1.0f, p.z + r, u1, v0);
		mTessellator.vertexUV(p.x + 0.0f, p.y + 1.0f, p.z + r, u0, v0);
		mTessellator.vertexUV(p.x + 0.0f, p.y + 0.0f, p.z + r, u0, v1);

		mTessellator.vertexUV(p.x + 0.0f, p.y + 0.0f, p.z + r, u0, v1);
		mTessellator.vertexUV(p.x + 0.0f, p.y + 1.0f, p.z + r, u0, v0);
		mTessellator.vertexUV(p.x + 1.0f, p.y + 1.0f, p.z + r, u1, v0);
		mTessellator.vertexUV(p.x + 1.0f, p.y + 0.0f, p.z + r, u1, v1);
	}

	if((facings& VineBlock::VINE_SOUTH) != 0) {
		mTessellator.vertexUV(p.x + 1.0f, p.y + 1.0f, p.z + 1.0f - r, u0, v0);
		mTessellator.vertexUV(p.x + 1.0f, p.y + 0.0f, p.z + 1.0f - r, u0, v1);
		mTessellator.vertexUV(p.x + 0.0f, p.y + 0.0f, p.z + 1.0f - r, u1, v1);
		mTessellator.vertexUV(p.x + 0.0f, p.y + 1.0f, p.z + 1.0f - r, u1, v0);

		mTessellator.vertexUV(p.x + 0.0f, p.y + 1.0f, p.z + 1.0f - r, u1, v0);
		mTessellator.vertexUV(p.x + 0.0f, p.y + 0.0f, p.z + 1.0f - r, u1, v1);
		mTessellator.vertexUV(p.x + 1.0f, p.y + 0.0f, p.z + 1.0f - r, u0, v1);
		mTessellator.vertexUV(p.x + 1.0f, p.y + 1.0f, p.z + 1.0f - r, u0, v0);
	}

	auto aboveBlock = mRegion->getBlockID(p.x, p.y + 1, p.z);
	if(Block::mSolid[aboveBlock]) {
		mTessellator.vertexUV(p.x + 1.0f, p.y + 1 - r, p.z + 0.0f, u0, v0);
		mTessellator.vertexUV(p.x + 1.0f, p.y + 1 - r, p.z + 1.0f, u0, v1);
		mTessellator.vertexUV(p.x + 0.0f, p.y + 1 - r, p.z + 1.0f, u1, v1);
		mTessellator.vertexUV(p.x + 0.0f, p.y + 1 - r, p.z + 0.0f, u1, v0);
	}

	return true;
}

bool BlockTessellator::tessellateCocoaInWorld(const CocoaBlock& block, const BlockPos& p, DataID data) {
	mTessellator.color(Color::WHITE);
	_tex1(toUV(getLightColor(p)));

	int dir = Block::mCocoa->getBlockState(BlockState::Direction).get<int>(data);
	int age = CocoaBlock::getAge(data);
	auto& cocoaTex = BlockGraphics::mBlocks[block.getId()]->getTexture(0, age);
	auto& tex = mUseFixedTexture ? mFixedTexture : cocoaTex;
	// Not all blocks are 16x16 anymore
	float scale = tex.pixelWidth() / 16.f;

	int cocoaWidth = 4 + age * 2;
	int cocoaHeight = 5 + age * 2;

	float us = 15.0f * scale - cocoaWidth;
	float ue = 15.0f * scale;
	float vs = 4.0f * scale;
	float ve = 4.0f * scale + cocoaHeight;

	float u0 = tex.offsetWidthPixel(us) + 0.0002f;
	float u1 = tex.offsetWidthPixel(ue) - 0.0002f;
	float v0 = tex.offsetHeightPixel(vs) + 0.0002f;
	float v1 = tex.offsetHeightPixel(ve) - 0.0002f;

	float offX = 0;
	float offZ = 0;

	switch(dir) {
	case Direction::NORTH:
		offX = 8.0f - cocoaWidth / 2;
		offZ = 1.0f;
		break;
	case Direction::SOUTH:
		offX = 8.0f - cocoaWidth / 2;
		offZ = 15.0f - cocoaWidth;
		break;
	case Direction::EAST:
		offX = 15.0f - cocoaWidth;
		offZ = 8.0f - cocoaWidth / 2;
		break;
	case Direction::WEST:
		offX = 1.0f;
		offZ = 8.0f - cocoaWidth / 2;
		break;
	}

	float x0 = p.x + offX / 16.0f;
	float x1 = p.x + (offX + cocoaWidth) / 16.0f;
	float y0 = p.y + (12.0f - cocoaHeight) / 16.0f;
	float y1 = p.y + 12.0f / 16.0f;
	float z0 = p.z + offZ / 16.0f;
	float z1 = p.z + (offZ + cocoaWidth) / 16.0f;

	// west
	{
		mTessellator.vertexUV(x0, y0, z0, u0, v1);
		mTessellator.vertexUV(x0, y0, z1, u1, v1);
		mTessellator.vertexUV(x0, y1, z1, u1, v0);
		mTessellator.vertexUV(x0, y1, z0, u0, v0);
	}
	// east
	{
		mTessellator.vertexUV(x1, y0, z1, u0, v1);
		mTessellator.vertexUV(x1, y0, z0, u1, v1);
		mTessellator.vertexUV(x1, y1, z0, u1, v0);
		mTessellator.vertexUV(x1, y1, z1, u0, v0);
	}
	// north
	{
		mTessellator.vertexUV(x1, y0, z0, u0, v1);
		mTessellator.vertexUV(x0, y0, z0, u1, v1);
		mTessellator.vertexUV(x0, y1, z0, u1, v0);
		mTessellator.vertexUV(x1, y1, z0, u0, v0);
	}
	// south
	{
		mTessellator.vertexUV(x0, y0, z1, u0, v1);
		mTessellator.vertexUV(x1, y0, z1, u1, v1);
		mTessellator.vertexUV(x1, y1, z1, u1, v0);
		mTessellator.vertexUV(x0, y1, z1, u0, v0);
	}

	int topWidth = cocoaWidth;
	if(age >= 2) {
		// special case because the top piece didn't fit
		topWidth--;
	}

	u0 = tex._u0;
	u1 = tex.offsetWidthPixel((float)topWidth);
	v0 = tex._v0;
	v1 = tex.offsetHeightPixel((float)topWidth);

	// top
	{
		mTessellator.vertexUV(x0, y1, z1, u0, v1);
		mTessellator.vertexUV(x1, y1, z1, u1, v1);
		mTessellator.vertexUV(x1, y1, z0, u1, v0);
		mTessellator.vertexUV(x0, y1, z0, u0, v0);
	}
	// bottom
	{
		mTessellator.vertexUV(x0, y0, z0, u0, v0);
		mTessellator.vertexUV(x1, y0, z0, u1, v0);
		mTessellator.vertexUV(x1, y0, z1, u1, v1);
		mTessellator.vertexUV(x0, y0, z1, u0, v1);
	}

	// stalk
	TextureUVCoordinateSet stalk = cocoaTex.subTexture(12 * scale, 0, 4 * (int)scale, 4 * (int)scale);
	float offset = (4.0f / 16.0f);

	if(dir == Direction::NORTH) {
		BlockTextureTessellator::render(mTessellator, Vec3((float)p.x, (float)p.y, (float)p.z - offset), stalk, true, 0, 270.0f, Vec3::ZERO, false, false, tex.pixelWidth());
	} else if(dir == Direction::WEST) {
		BlockTextureTessellator::render(mTessellator, Vec3((float)p.x - offset, (float)p.y, (float)p.z), stalk, true, 0, 0, Vec3::ZERO, false, false, tex.pixelWidth());
	} else if(dir == Direction::SOUTH) {
		BlockTextureTessellator::render(mTessellator, Vec3((float)p.x, (float)p.y, (float)p.z + offset), stalk, true, 0, 90, Vec3::ZERO, false, false, tex.pixelWidth());
	} else if(dir == Direction::EAST) {
		BlockTextureTessellator::render(mTessellator, Vec3((float)p.x + offset, (float)p.y, (float)p.z), stalk, true, 0, 180, Vec3::ZERO, false, false, tex.pixelWidth());
	}

	return true;
}

bool BlockTessellator::tessellateEndPortalFrameInWorld(const EndPortalFrameBlock& block, const BlockPos& pos, DataID data) {
	int direction = Block::mEndPortalFrame->getBlockState(BlockState::Direction).get<int>(data);
	if (direction == Direction::SOUTH) {
		mFlipFace[Facing::UP] = FLIP_180;
	} else if (direction == Direction::EAST) {
		mFlipFace[Facing::UP] = FLIP_CW;
	} else if (direction == Direction::WEST) {
		mFlipFace[Facing::UP] = FLIP_CCW;
	}

	if (!EndPortalFrameBlock::hasEye(data)) {
		mCurrentShape.set(0, 0, 0, 1, 13.0f / 16.0f, 1);
		tessellateBlockInWorld(block, pos, data);

		mFlipFace[Facing::UP] = FLIP_NONE;
		return true;
	}

	int daFlip = mFlipFace[Facing::UP];	// keep flip because it's reset after the first render call

	mCurrentShape.set(0, 0, 0, 1, 13.0f / 16.0f, 1);
	tessellateBlockInWorld(block, pos, data);

	mFlipFace[Facing::UP] = daFlip;
	mUseFixedTexture = true;
	static const FacingID EYE_TEXTURE = Facing::UP;
	mFixedTexture = BlockGraphics::mBlocks[block.getId()]->getCarriedTexture(EYE_TEXTURE, 0);
	mCurrentShape.set(4.0f / 16.0f, 13.0f / 16.0f, 4.0f / 16.0f, 12.0f / 16.0f, 1, 12.0f / 16.0f);
	tessellateBlockInWorld(block, pos, data);
	mUseFixedTexture = false;

	mFlipFace[Facing::UP] = FLIP_NONE;

	return true;
}

bool BlockTessellator::tessellateStairsInWorld(const StairBlock& block, const BlockPos& p, int data){
	block.setBaseShape(*mRegion, p, mCurrentShape, true);
	tessellateBlockInWorld(block, p, data);

	bool checkInnerPiece = block.setStepShape(*mRegion, p, mCurrentShape, true);
	tessellateBlockInWorld(block, p, data);

	if (checkInnerPiece) {
		if (block.setInnerPieceShape(*mRegion, p, mCurrentShape, true)) {
			tessellateBlockInWorld(block, p, data);
		}
	}

	return true;
}

bool BlockTessellator::tessellateDoorInWorld(const Block& block, const BlockPos& p, int data) {
	static const FacingID LOWER = Facing::UP;
	static const FacingID UPPER = Facing::NORTH;
	
	bool changed = false;

	auto centerColor = getLightColor(p);

	bool isUpper = Block::mWoodenDoor->getBlockState(BlockState::UpperBlockBit).getBool(data);
	auto exceptSide = isUpper ? Facing::DOWN : Facing::UP;

	BlockOccluder occluder(mBlockCache, block, p, mCurrentShape, Facing::ALL_EXCEPT[exceptSide], mRenderingGUI);

	if (mUseFixedTexture) {
		mCurrentShape = mCurrentShape.grow(Vec3(0.001f, 0.0f, 0.001f));
	}

	if(!occluder.occludes(Facing::DOWN)) {
		_tex1(toUV(mCurrentShape.min.y > 0 ? centerColor : getLightColor(p.below())));
		mTessellator.color(Color::SHADE_DOWN);
		renderFaceDown(block, p, _getTexture(block, LOWER, data));
		changed = true;
	}

	if(!occluder.occludes(Facing::UP)) {
		_tex1(toUV(mCurrentShape.max.y < 1 ? centerColor : getLightColor(p.above())));
		mTessellator.color(Color::SHADE_UP);
		renderFaceUp(block, p, _getTexture(block, LOWER, data));
		changed = true;
	}

	if(!occluder.occludes(Facing::NORTH)) {
		_tex1(toUV(mCurrentShape.min.z > 0 ? centerColor : getLightColor(p.north())));
		mTessellator.color(Color::SHADE_NORTH_SOUTH);
		TextureUVCoordinateSet tex = mUseFixedTexture ? mFixedTexture : _getTexture(block, isUpper ? UPPER : LOWER, data);
		if (((const DoorBlock*)Block::mWoodenDoor)->shouldFlipTexture(*mRegion, p, Facing::NORTH)) {
			tex = tex.getFlippedHorizontal();
		}

		renderNorth(block, p, tex);
		changed = true;
	}

	if(!occluder.occludes(Facing::SOUTH)) {
		_tex1(toUV(mCurrentShape.max.z < 1 ? centerColor : getLightColor(p.south())));
		mTessellator.color(Color::SHADE_NORTH_SOUTH);
		TextureUVCoordinateSet tex = mUseFixedTexture ? mFixedTexture : _getTexture(block, isUpper ? UPPER : LOWER, data);
		if (((const DoorBlock*)Block::mWoodenDoor)->shouldFlipTexture(*mRegion, p, Facing::SOUTH)) {
			tex = tex.getFlippedHorizontal();
		}

		renderSouth(block, p, tex);
		changed = true;
	}

	if(!occluder.occludes(Facing::WEST)) {
		_tex1(toUV(mCurrentShape.min.x > 0 ? centerColor : getLightColor(p.west())));
		mTessellator.color(Color::SHADE_WEST_EAST);
		TextureUVCoordinateSet tex = mUseFixedTexture ? mFixedTexture : _getTexture(block, isUpper ? UPPER : LOWER, data);
		if (((const DoorBlock*)Block::mWoodenDoor)->shouldFlipTexture(*mRegion, p, Facing::WEST)) {
			tex = tex.getFlippedHorizontal();
		}

		renderWest(block, p, tex);
		changed = true;
	}

	if(!occluder.occludes(Facing::EAST)) {
		_tex1(toUV(mCurrentShape.max.x < 1 ? centerColor : getLightColor(p.east())));
		mTessellator.color(Color::SHADE_WEST_EAST);

		TextureUVCoordinateSet tex = mUseFixedTexture ? mFixedTexture : _getTexture(block, isUpper ? UPPER : LOWER, data);
		if (((const DoorBlock*)Block::mWoodenDoor)->shouldFlipTexture(*mRegion, p, Facing::EAST)) {
			tex = tex.getFlippedHorizontal();
		}

		renderEast(block, p, tex);
		changed = true;
	}

	return changed;
}

bool BlockTessellator::tessellateRowInWorld(const Block& block, const BlockPos& pos, DataID data ) {
	_tex1(toUV(getLightColor(pos)));
	mTessellator.color(Color::WHITE);
	tessellateRowTexture(block, data, float(pos.x), pos.y - 1 / 16.0f, float(pos.z));
	return true;
}

void BlockTessellator::_randomizeFaceDirection(const Block& block, FacingID face, const BlockPos& p) {

	const BlockGraphics& blockGraphics = *BlockGraphics::mBlocks[block.mID];
	if (!mRenderingGUI && blockGraphics.isTextureIsotropic(face) && mFlipFace[face] == 0) {
		int32_t seed = (p.x * 3129871) ^ (p.z * 116129781l) ^ p.y;
		seed = seed * seed * 42317861 + seed * 11;
		mFlipFace[face] = (seed >> 24) & 0x3;
	}
}

void BlockTessellator::renderFaceDown(const Block& block, const Vec3& p, const TextureUVCoordinateSet& inTex) {

	TextureUVCoordinateSet tex = mUseFixedTexture ? mFixedTexture : inTex;

	mTessellator.quadFacing(Facing::DOWN);
	float u00 = tex.offsetWidth(mCurrentShape.min.x);//(xt + mCurrentShape.min.x * 16) / 256.0f;
	float u11 = tex.offsetWidth(mCurrentShape.max.x);//(xt + mCurrentShape.max.x * 16 - 0.01f) / 256.0f;
	float v00 = tex.offsetHeight(mCurrentShape.min.z);	//(yt + mCurrentShape.min.z * 16) / 256.0f;
	float v11 = tex.offsetHeight(mCurrentShape.max.z);	//(yt + mCurrentShape.max.z * 16 - 0.01f) / 256.0f;

	if (mCurrentShape.min.x < 0 || mCurrentShape.max.x > 1) {
		u11 = tex._u0 + u11 - u00;
		u00 = tex._u0;
	}

	if (mCurrentShape.min.z < 0 || mCurrentShape.max.z > 1) {
		v11 = tex._v0 + v11 - v00;
		v00 = tex._v0;
	}

	float u01 = u11, u10 = u00, v01 = v00, v10 = v11;

	_randomizeFaceDirection(block, Facing::DOWN, p);

	auto& downFlip = mFlipFace[Facing::DOWN];
	if (downFlip == FLIP_CCW) {
		u00 = tex.offsetWidth(mCurrentShape.min.z);	// tex.getU(blockShapeZ0 * 16f);
		v00 = tex.inverseOffsetHeight(mCurrentShape.max.x);	// tex.getV(SharedConstants.WORLD_RESOLUTION - blockShapeX1 * 16f);
		u11 = tex.offsetWidth(mCurrentShape.max.z);	// tex.getU(blockShapeZ1 * 16f);
		v11 = tex.inverseOffsetHeight(mCurrentShape.min.x);	// tex.getV(SharedConstants.WORLD_RESOLUTION - blockShapeX0 * 16f);

		u01 = u11;
		u10 = u00;
		v01 = v00;
		v10 = v11;
		u01 = u00;
		u10 = u11;
		v00 = v11;
		v11 = v01;
	} else if (downFlip == FLIP_CW) {
		// reshape
		u00 = tex.inverseOffsetWidth(mCurrentShape.max.z);	// tex.getU(SharedConstants.WORLD_RESOLUTION - blockShapeZ1 * 16f);
		v00 = tex.offsetHeight(mCurrentShape.min.x);// tex.getV(blockShapeX0 * 16f);
		u11 = tex.inverseOffsetWidth(mCurrentShape.min.z);	// tex.getU(SharedConstants.WORLD_RESOLUTION - blockShapeZ0 * 16f);
		v11 = tex.offsetHeight(mCurrentShape.max.x);// tex.getV(blockShapeX1 * 16f);

		// rotate
		u01 = u11;
		u10 = u00;
		v01 = v00;
		v10 = v11;
		u00 = u01;
		u11 = u10;
		v01 = v11;
		v10 = v00;
	} else if (downFlip == FLIP_180) {
		u00 = tex.inverseOffsetWidth(mCurrentShape.min.x);	// tex.getU(SharedConstants.WORLD_RESOLUTION - blockShapeX0 * 16f);
		u11 = tex.inverseOffsetWidth(mCurrentShape.max.x);	// tex.getU(SharedConstants.WORLD_RESOLUTION - blockShapeX1 * 16f);
		v00 = tex.inverseOffsetHeight(mCurrentShape.min.z);	// tex.getV(SharedConstants.WORLD_RESOLUTION - blockShapeZ0 * 16f);
		v11 = tex.inverseOffsetHeight(mCurrentShape.max.z);	// tex.getV(SharedConstants.WORLD_RESOLUTION - blockShapeZ1 * 16f);

		u01 = u11;
		u10 = u00;
		v01 = v00;
		v10 = v11;
	}

//#error mesh has the position inside the chunk as offset...
	Vec3 a = p + mCurrentShape.min, b = p + mCurrentShape.max;

	if (mUseNormals) {
		mTessellator.normal(Vec3::NEG_UNIT_Y);
	}

	if (mApplyAmbientOcclusion) {

		std::array<Color, 4> corners = { mAoColors[0], mAoColors[3], mAoColors[1], mAoColors[2] };

		mTessellator.color(Color::bilinear(corners, mCurrentShape.min.x, mCurrentShape.max.z));
		_tex1(toUV(mTc[1]));
		mTessellator.vertexUV(a.x, a.y, b.z, u10, v10);

		mTessellator.color(Color::bilinear(corners, mCurrentShape.min.x, mCurrentShape.min.z));
		_tex1(toUV(mTc[0]));
		mTessellator.vertexUV(a.x, a.y, a.z, u00, v00);

		mTessellator.color(Color::bilinear(corners, mCurrentShape.max.x, mCurrentShape.min.z));
		_tex1(toUV(mTc[3]));
		mTessellator.vertexUV(b.x, a.y, a.z, u01, v01);

		mTessellator.color(Color::bilinear(corners, mCurrentShape.max.x, mCurrentShape.max.z));
		_tex1(toUV(mTc[2]));
		mTessellator.vertexUV(b.x, a.y, b.z, u11, v11);
	} else {
		mTessellator.vertexUV(a.x, a.y, b.z, u10, v10);
		mTessellator.vertexUV(a.x, a.y, a.z, u00, v00);
		mTessellator.vertexUV(b.x, a.y, a.z, u01, v01);
		mTessellator.vertexUV(b.x, a.y, b.z, u11, v11);
	}
}

void BlockTessellator::_tex1(const Vec2& uv) {
	if (!mRenderingGUI) {
		mTessellator.tex1(uv);
	}
}

void BlockTessellator::renderFaceUp(const Block& block, const Vec3& p, const TextureUVCoordinateSet& inTex) {

	TextureUVCoordinateSet tex = mUseFixedTexture ? mFixedTexture : inTex;

	mTessellator.quadFacing(Facing::UP);

	float u00 = tex.offsetWidth(mCurrentShape.min.x);//(xt + mCurrentShape.min.x * 16) / 256.0f;
	float u11 = tex.offsetWidth(mCurrentShape.max.x);//(xt + mCurrentShape.max.x * 16 - 0.01f) / 256.0f;
	float v00 = tex.offsetHeight(mCurrentShape.min.z);	//(yt + mCurrentShape.min.z * 16) / 256.0f;
	float v11 = tex.offsetHeight(mCurrentShape.max.z);	//(yt + mCurrentShape.max.z * 16 - 0.01f) / 256.0f;

	if (mCurrentShape.min.x < 0 || mCurrentShape.max.x > 1) {
		u11 = tex._u0 + u11 - u00;
		u00 = tex._u0;
	}

	if (mCurrentShape.min.z < 0 || mCurrentShape.max.z > 1) {
		v11 = tex._v0 + v11 - v00;
		v00 = tex._v0;
	}

	float u01 = u11, u10 = u00, v01 = v00, v10 = v11;

	_randomizeFaceDirection(block, Facing::UP, p);

	auto& upFlip = mFlipFace[Facing::UP];
	if (upFlip == FLIP_CW) {
		u00 = tex.offsetWidth(mCurrentShape.min.z);	// tex.getU(blockShapeZ0 * 16f);
		v00 = tex.inverseOffsetHeight(mCurrentShape.max.x);	// tex.getV(SharedConstants.WORLD_RESOLUTION - blockShapeX1 * 16f);
		u11 = tex.offsetWidth(mCurrentShape.max.z);	// tex.getU(blockShapeZ1 * 16f);
		v11 = tex.inverseOffsetHeight(mCurrentShape.min.x);	// tex.getV(SharedConstants.WORLD_RESOLUTION - blockShapeX0 * 16f);

		u01 = u11;
		u10 = u00;
		v01 = v00;
		v10 = v11;
		u01 = u00;
		u10 = u11;
		v00 = v11;
		v11 = v01;
	} else if (upFlip == FLIP_CCW) {
		// reshape
		u00 = tex.inverseOffsetWidth(mCurrentShape.max.z);	// tex.getU(SharedConstants.WORLD_RESOLUTION - blockShapeZ1 * 16f);
		v00 = tex.offsetHeight(mCurrentShape.min.x);// tex.getV(blockShapeX0 * 16f);
		u11 = tex.inverseOffsetWidth(mCurrentShape.min.z);	// tex.getU(SharedConstants.WORLD_RESOLUTION - blockShapeZ0 * 16f);
		v11 = tex.offsetHeight(mCurrentShape.max.x);// tex.getV(blockShapeX1 * 16f);

		// rotate
		u01 = u11;
		u10 = u00;
		v01 = v00;
		v10 = v11;
		u00 = u01;
		u11 = u10;
		v01 = v11;
		v10 = v00;
	} else if (upFlip == FLIP_180) {
		u00 = tex.inverseOffsetWidth(mCurrentShape.min.x);	// tex.getU(SharedConstants.WORLD_RESOLUTION - blockShapeX0 * 16f);
		u11 = tex.inverseOffsetWidth(mCurrentShape.max.x);	// tex.getU(SharedConstants.WORLD_RESOLUTION - blockShapeX1 * 16f);
		v00 = tex.inverseOffsetHeight(mCurrentShape.min.z);	// tex.getV(SharedConstants.WORLD_RESOLUTION - blockShapeZ0 * 16f);
		v11 = tex.inverseOffsetHeight(mCurrentShape.max.z);	// tex.getV(SharedConstants.WORLD_RESOLUTION - blockShapeZ1 * 16f);

		u01 = u11;
		u10 = u00;
		v01 = v00;
		v10 = v11;
	}

	Vec3 a = p + mCurrentShape.min, b = p + mCurrentShape.max;

	if (mUseNormals) {
		mTessellator.normal(Vec3::UNIT_Y);
	}

	if(mApplyAmbientOcclusion) {
		
		std::array<Color, 4> corners = { mAoColors[3], mAoColors[0], mAoColors[2], mAoColors[1] };

		mTessellator.color(Color::bilinear(corners, mCurrentShape.max.x, mCurrentShape.max.z));
		_tex1(toUV(mTc[1]));
		mTessellator.vertexUV(b.x, b.y, b.z, u11, v11);

		mTessellator.color(Color::bilinear(corners, mCurrentShape.max.x, mCurrentShape.min.z));
		_tex1(toUV(mTc[0]));
		mTessellator.vertexUV(b.x, b.y, a.z, u01, v01);

		mTessellator.color(Color::bilinear(corners, mCurrentShape.min.x, mCurrentShape.min.z));
		_tex1(toUV(mTc[3]));
		mTessellator.vertexUV(a.x, b.y, a.z, u00, v00);

		mTessellator.color(Color::bilinear(corners, mCurrentShape.min.x, mCurrentShape.max.z));
		_tex1(toUV(mTc[2]));
		mTessellator.vertexUV(a.x, b.y, b.z, u10, v10);
	} else {	//this is usually used for stand-alone blocks, add the normal
		mTessellator.vertexUV(b.x, b.y, b.z, u11, v11);
		mTessellator.vertexUV(b.x, b.y, a.z, u01, v01);
		mTessellator.vertexUV(a.x, b.y, a.z, u00, v00);
		mTessellator.vertexUV(a.x, b.y, b.z, u10, v10);
	}
}

void BlockTessellator::renderNorth(const Block& block, const Vec3& p, const TextureUVCoordinateSet& inTex) {
	TextureUVCoordinateSet tex = mUseFixedTexture ? mFixedTexture : inTex;

	mTessellator.quadFacing(Facing::NORTH);

	float u00 = tex.offsetWidth(mCurrentShape.min.x);//(xt + mCurrentShape.min.x * 16) / 256.0f;
	float u11 = tex.offsetWidth(mCurrentShape.max.x);//(xt + mCurrentShape.max.x * 16 - 0.01f) / 256.0f;
	float v00 = tex.inverseOffsetHeight(mCurrentShape.max.y);//(yt + 16 - mCurrentShape.max.y * 16) / 256.0f;
	float v11 = tex.inverseOffsetHeight(mCurrentShape.min.y);//(yt + 16 - mCurrentShape.min.y * 16 - 0.01f) / 256.0f;
	if (mXFlipTexture) {
		float tmp = u00;
		u00 = u11;
		u11 = tmp;
	}

	if (mCurrentShape.min.x < 0 || mCurrentShape.max.x > 1) {
		u11 = tex._u0 + u11 - u00;
		u00 = tex._u0;
	}

	if (mCurrentShape.min.y < 0 || mCurrentShape.max.y > 1) {
		v11 = tex._v0 + v11 - v00;
		v00 = tex._v0;
	}

	float u01 = u11, u10 = u00, v01 = v00, v10 = v11;

	_randomizeFaceDirection(block, Facing::NORTH, p);

	auto& northFlip = mFlipFace[Facing::NORTH];
	if (northFlip == FLIP_CCW) {
		u00 = tex.offsetWidth(mCurrentShape.min.y);	// tex.getU(blockShapeY0 * 16f);
		v00 = tex.inverseOffsetHeight(mCurrentShape.min.x);	// tex.getV(SharedConstants.WORLD_RESOLUTION - blockShapeX0 * 16f);
		u11 = tex.offsetWidth(mCurrentShape.max.y);	// tex.getU(blockShapeY1 * 16f);
		v11 = tex.inverseOffsetHeight(mCurrentShape.max.x);	// tex.getV(SharedConstants.WORLD_RESOLUTION - blockShapeX1 * 16f);

		u01 = u11;
		u10 = u00;
		v01 = v00;
		v10 = v11;
		u01 = u00;
		u10 = u11;
		v00 = v11;
		v11 = v01;
	} else if (northFlip == FLIP_CW) {
		// reshape
		u00 = tex.inverseOffsetWidth(mCurrentShape.max.y);	// tex.getU(SharedConstants.WORLD_RESOLUTION - blockShapeY1 * 16f);
		v00 = tex.offsetHeight(mCurrentShape.max.x);// tex.getV(blockShapeX1 * 16f);
		u11 = tex.inverseOffsetWidth(mCurrentShape.min.y);	// tex.getU(SharedConstants.WORLD_RESOLUTION - blockShapeY0 * 16f);
		v11 = tex.offsetHeight(mCurrentShape.min.x);// tex.getV(blockShapeX0 * 16f);

		// rotate
		u01 = u11;
		u10 = u00;
		v01 = v00;
		v10 = v11;
		u00 = u01;
		u11 = u10;
		v01 = v11;
		v10 = v00;
	} else if (northFlip == FLIP_180) {
		u00 = tex.inverseOffsetWidth(mCurrentShape.min.x);	// tex.getU(SharedConstants.WORLD_RESOLUTION - blockShapeX0 * 16f);
		u11 = tex.inverseOffsetWidth(mCurrentShape.max.x);	// tex.getU(SharedConstants.WORLD_RESOLUTION - blockShapeX1 * 16f);
		v00 = tex.offsetHeight(mCurrentShape.max.y);// tex.getV(blockShapeY1 * 16f);
		v11 = tex.offsetHeight(mCurrentShape.min.y);// tex.getV(blockShapeY0 * 16f);

		u01 = u11;
		u10 = u00;
		v01 = v00;
		v10 = v11;
	}

	Vec3 a = p + mCurrentShape.min, b = p + mCurrentShape.max;

	if (mUseNormals) {
		mTessellator.normal(Vec3::NEG_UNIT_Z);
	}

	if (mApplyAmbientOcclusion) {

		std::array<Color, 4> corners = { mAoColors[1], mAoColors[2], mAoColors[0 + 4], mAoColors[3 + 4]};

		mTessellator.color(Color::bilinear(corners, mCurrentShape.min.x, mCurrentShape.max.y));
		_tex1(toUV(mTc[0]));
		mTessellator.vertexUV(a.x, b.y, a.z, u01, v01);

		mTessellator.color(Color::bilinear(corners, mCurrentShape.max.x, mCurrentShape.max.y));
		_tex1(toUV(mTc[3]));
		mTessellator.vertexUV(b.x, b.y, a.z, u00, v00);

		mTessellator.color(Color::bilinear(corners, mCurrentShape.max.x, mCurrentShape.min.y));
		_tex1(toUV(mTc[2]));
		mTessellator.vertexUV(b.x, a.y, a.z, u10, v10);

		mTessellator.color(Color::bilinear(corners, mCurrentShape.min.x, mCurrentShape.min.y));
		_tex1(toUV(mTc[1]));
		mTessellator.vertexUV(a.x, a.y, a.z, u11, v11);
	} else {
		mTessellator.vertexUV(a.x, b.y, a.z, u01, v01);
		mTessellator.vertexUV(b.x, b.y, a.z, u00, v00);
		mTessellator.vertexUV(b.x, a.y, a.z, u10, v10);
		mTessellator.vertexUV(a.x, a.y, a.z, u11, v11);
	}
}

void BlockTessellator::renderSouth(const Block& block, const Vec3& p, const TextureUVCoordinateSet& inTex) {

	TextureUVCoordinateSet tex = mUseFixedTexture ? mFixedTexture : inTex;

	mTessellator.quadFacing(Facing::SOUTH);

	float u00 = tex.offsetWidth(mCurrentShape.min.x);//(xt + mCurrentShape.min.x * 16) / 256.0f;
	float u11 = tex.offsetWidth(mCurrentShape.max.x);//(xt + mCurrentShape.max.x * 16 - 0.01f) / 256.0f;
	float v00 = tex.inverseOffsetHeight(mCurrentShape.max.y);//(yt + 16 - mCurrentShape.max.y * 16) / 256.0f;
	float v11 = tex.inverseOffsetHeight(mCurrentShape.min.y);//(yt + 16 - mCurrentShape.min.y * 16 - 0.01f) / 256.0f;
	if (mXFlipTexture) {
		float tmp = u00;
		u00 = u11;
		u11 = tmp;
	}

	if (mCurrentShape.min.x < 0 || mCurrentShape.max.x > 1) {
		u11 = tex._u0 + u11 - u00;
		u00 = tex._u0;
	}

	if (mCurrentShape.min.y < 0 || mCurrentShape.max.y > 1) {
		v11 = tex._v0 + v11 - v00;
		v00 = tex._v0;
	}

	float u01 = u11, u10 = u00, v01 = v00, v10 = v11;

	_randomizeFaceDirection(block, Facing::SOUTH, p);

	auto& southFlip = mFlipFace[Facing::SOUTH];
	if (southFlip == FLIP_CW) {
		u00 = tex.offsetWidth(mCurrentShape.min.y);	// tex.getU(blockShapeY0 * 16f);
		v11 = tex.inverseOffsetHeight(mCurrentShape.min.x);	// tex.getV(SharedConstants.WORLD_RESOLUTION - blockShapeX0 * 16f);
		u11 = tex.offsetWidth(mCurrentShape.max.y);	// tex.getU(blockShapeY1 * 16f);
		v00 = tex.inverseOffsetHeight(mCurrentShape.max.x);	// tex.getV(SharedConstants.WORLD_RESOLUTION - blockShapeX1 * 16f);

		u01 = u11;
		u10 = u00;
		v01 = v00;
		v10 = v11;
		u01 = u00;
		u10 = u11;
		v00 = v11;
		v11 = v01;
	} else if (southFlip == FLIP_CCW) {
		// reshape
		u00 = tex.inverseOffsetWidth(mCurrentShape.max.y);	// tex.getU(SharedConstants.WORLD_RESOLUTION - blockShapeY1 * 16f);
		v00 = tex.offsetHeight(mCurrentShape.min.x);// tex.getV(blockShapeX0 * 16f);
		u11 = tex.inverseOffsetWidth(mCurrentShape.min.y);	// tex.getU(SharedConstants.WORLD_RESOLUTION - blockShapeY0 * 16f);
		v11 = tex.offsetHeight(mCurrentShape.max.x);// tex.getV(blockShapeX1 * 16f);

		// rotate
		u01 = u11;
		u10 = u00;
		v01 = v00;
		v10 = v11;
		u00 = u01;
		u11 = u10;
		v01 = v11;
		v10 = v00;
	} else if (southFlip == FLIP_180) {
		u00 = tex.inverseOffsetWidth(mCurrentShape.min.x);	// tex.getU(SharedConstants.WORLD_RESOLUTION - blockShapeX0 * 16f);
		u11 = tex.inverseOffsetWidth(mCurrentShape.max.x);	// tex.getU(SharedConstants.WORLD_RESOLUTION - blockShapeX1 * 16f);
		v00 = tex.offsetHeight(mCurrentShape.max.y);// tex.getV(blockShapeY1 * 16f);
		v11 = tex.offsetHeight(mCurrentShape.min.y);// tex.getV(blockShapeY0 * 16f);

		u01 = u11;
		u10 = u00;
		v01 = v00;
		v10 = v11;
	}

	Vec3 a = p + mCurrentShape.min, b = p + mCurrentShape.max;

	if (mUseNormals) {
		mTessellator.normal(Vec3::UNIT_Z);
	}

	if (mApplyAmbientOcclusion) {

		std::array<Color, 4> corners = { mAoColors[2], mAoColors[1], mAoColors[3 + 4], mAoColors[0 + 4] };

		mTessellator.color(Color::bilinear(corners, mCurrentShape.min.x, mCurrentShape.max.y));
		_tex1(toUV(mTc[3]));
		mTessellator.vertexUV(a.x, b.y, b.z, u00, v00);

		mTessellator.color(Color::bilinear(corners, mCurrentShape.min.x, mCurrentShape.min.y));
		_tex1(toUV(mTc[2]));
		mTessellator.vertexUV(a.x, a.y, b.z, u10, v10);

		mTessellator.color(Color::bilinear(corners, mCurrentShape.max.x, mCurrentShape.min.y));
		_tex1(toUV(mTc[1]));
		mTessellator.vertexUV(b.x, a.y, b.z, u11, v11);

		mTessellator.color(Color::bilinear(corners, mCurrentShape.max.x, mCurrentShape.max.y));
		_tex1(toUV(mTc[0]));
		mTessellator.vertexUV(b.x, b.y, b.z, u01, v01);

	} else {
		mTessellator.vertexUV(a.x, b.y, b.z, u00, v00);
		mTessellator.vertexUV(a.x, a.y, b.z, u10, v10);
		mTessellator.vertexUV(b.x, a.y, b.z, u11, v11);
		mTessellator.vertexUV(b.x, b.y, b.z, u01, v01);
	}
}

void BlockTessellator::renderWest(const Block& block, const Vec3& p, const TextureUVCoordinateSet& inTex) {

	TextureUVCoordinateSet tex = mUseFixedTexture ? mFixedTexture : inTex;

	mTessellator.quadFacing(Facing::WEST);

	float u00 = tex.offsetWidth(mCurrentShape.min.z);//(xt + mCurrentShape.min.z * 16) / 256.0f;
	float u11 = tex.offsetWidth(mCurrentShape.max.z);//(xt + mCurrentShape.max.z * 16 - 0.01f) / 256.0f;
	float v00 = tex.inverseOffsetHeight(mCurrentShape.max.y);//(yt + 16 - mCurrentShape.max.y * 16) / 256.0f;
	float v11 = tex.inverseOffsetHeight(mCurrentShape.min.y);//(yt + 16 - mCurrentShape.min.y * 16 - 0.01f) / 256.0f;
	if (mXFlipTexture) {
		float tmp = u00;
		u00 = u11;
		u11 = tmp;
	}

	if (mCurrentShape.min.z < 0 || mCurrentShape.max.z > 1) {
		u11 = tex._u0 + u11 - u00;
		u00 = tex._u0;
	}

	if (mCurrentShape.min.y < 0 || mCurrentShape.max.y > 1) {
		v11 = tex._v0 + v11 - v00;
		v00 = tex._v0;
	}

	float u01 = u11, u10 = u00, v01 = v00, v10 = v11;

	_randomizeFaceDirection(block, Facing::WEST, p);

	auto& westFlip = mFlipFace[Facing::WEST];
	if (westFlip == FLIP_CW) {
		u00 = tex.offsetWidth(mCurrentShape.min.y);	// tex.getU(blockShapeY0 * 16f);
		v00 = tex.inverseOffsetHeight(mCurrentShape.max.z);	// tex.getV(SharedConstants.WORLD_RESOLUTION - blockShapeZ1 * 16f);
		u11 = tex.offsetWidth(mCurrentShape.max.y);	// tex.getU(blockShapeY1 * 16f);
		v11 = tex.inverseOffsetHeight(mCurrentShape.min.z);	// tex.getV(SharedConstants.WORLD_RESOLUTION - blockShapeZ0 * 16f);

		u01 = u11;
		u10 = u00;
		v01 = v00;
		v10 = v11;
		u01 = u00;
		u10 = u11;
		v00 = v11;
		v11 = v01;
	} else if (westFlip == FLIP_CCW) {
		// reshape
		u00 = tex.inverseOffsetWidth(mCurrentShape.max.y);	// tex.getU(SharedConstants.WORLD_RESOLUTION - blockShapeY1 * 16f);
		v00 = tex.offsetHeight(mCurrentShape.min.z);// tex.getV(blockShapeZ0 * 16f);
		u11 = tex.inverseOffsetWidth(mCurrentShape.min.y);	// tex.getU(SharedConstants.WORLD_RESOLUTION - blockShapeY0 * 16f);
		v11 = tex.offsetHeight(mCurrentShape.max.z);// tex.getV(blockShapeZ1 * 16f);

		// rotate
		u01 = u11;
		u10 = u00;
		v01 = v00;
		v10 = v11;
		u00 = u01;
		u11 = u10;
		v01 = v11;
		v10 = v00;
	} else if (westFlip == FLIP_180) {
		u00 = tex.inverseOffsetWidth(mCurrentShape.min.z);	// tex.getU(SharedConstants.WORLD_RESOLUTION - blockShapeZ0 * 16f);
		u11 = tex.inverseOffsetWidth(mCurrentShape.max.z);	// tex.getU(SharedConstants.WORLD_RESOLUTION - blockShapeZ1 * 16f);
		v00 = tex.offsetHeight(mCurrentShape.max.y);// tex.getV(blockShapeY1 * 16f);
		v11 = tex.offsetHeight(mCurrentShape.min.y);// tex.getV(blockShapeY0 * 16f);

		u01 = u11;
		u10 = u00;
		v01 = v00;
		v10 = v11;
	}

	Vec3 a = p + mCurrentShape.min, b = p + mCurrentShape.max;

	if (mUseNormals) {
		mTessellator.normal(Vec3::NEG_UNIT_X);
	}

	if (mApplyAmbientOcclusion) {

		std::array<Color, 4> corners = { mAoColors[2], mAoColors[1], mAoColors[3 + 4], mAoColors[0 + 4] };

		mTessellator.color(Color::bilinear(corners, mCurrentShape.max.z, mCurrentShape.max.y));
		_tex1(toUV(mTc[0]));
		mTessellator.vertexUV(a.x, b.y, b.z, u01, v01);

		mTessellator.color(Color::bilinear(corners, mCurrentShape.min.z, mCurrentShape.max.y));
		_tex1(toUV(mTc[3]));
		mTessellator.vertexUV(a.x, b.y, a.z, u00, v00);

		mTessellator.color(Color::bilinear(corners, mCurrentShape.min.z, mCurrentShape.min.y));
		_tex1(toUV(mTc[2]));
		mTessellator.vertexUV(a.x, a.y, a.z, u10, v10);

		mTessellator.color(Color::bilinear(corners, mCurrentShape.max.z, mCurrentShape.min.y));
		_tex1(toUV(mTc[1]));
		mTessellator.vertexUV(a.x, a.y, b.z, u11, v11);
	} else {
		mTessellator.vertexUV(a.x, b.y, b.z, u01, v01);
		mTessellator.vertexUV(a.x, b.y, a.z, u00, v00);
		mTessellator.vertexUV(a.x, a.y, a.z, u10, v10);
		mTessellator.vertexUV(a.x, a.y, b.z, u11, v11);
	}
}

void BlockTessellator::renderEast(const Block& block, const Vec3& p, const TextureUVCoordinateSet& inTex) {
	TextureUVCoordinateSet tex = mUseFixedTexture ? mFixedTexture : inTex;

	mTessellator.quadFacing(Facing::EAST);

	float u00 = tex.offsetWidth(mCurrentShape.min.z);//(xt + mCurrentShape.min.z * 16) / 256.0f;
	float u11 = tex.offsetWidth(mCurrentShape.max.z);//(xt + mCurrentShape.max.z * 16 - 0.01f) / 256.0f;
	float v00 = tex.inverseOffsetHeight(mCurrentShape.max.y);//(yt + 16 - mCurrentShape.max.y * 16) / 256.0f;
	float v11 = tex.inverseOffsetHeight(mCurrentShape.min.y);//(yt + 16 - mCurrentShape.min.y * 16 - 0.01f) / 256.0f;
	if (mXFlipTexture) {
		float tmp = u00;
		u00 = u11;
		u11 = tmp;
	}

	if (mCurrentShape.min.z < 0 || mCurrentShape.max.z > 1) {
		u11 = tex._u0 + u11 - u00;
		u00 = tex._u0;
	}

	if (mCurrentShape.min.y < 0 || mCurrentShape.max.y > 1) {
		v11 = tex._v0 + v11 - v00;
		v00 = tex._v0;
	}

	float u01 = u11, u10 = u00, v01 = v00, v10 = v11;

	_randomizeFaceDirection(block, Facing::EAST, p);

	auto& eastFlip = mFlipFace[Facing::EAST];
	if (eastFlip == FLIP_CCW) {
		u00 = tex.offsetWidth(mCurrentShape.min.y);	// tex.getU(blockShapeY0 * 16f);
		v00 = tex.inverseOffsetHeight(mCurrentShape.min.z);	// tex.getV(SharedConstants.WORLD_RESOLUTION - blockShapeZ0 * 16f);
		u11 = tex.offsetWidth(mCurrentShape.max.y);	// tex.getU(blockShapeY1 * 16f);
		v11 = tex.inverseOffsetHeight(mCurrentShape.max.z);	// tex.getV(SharedConstants.WORLD_RESOLUTION - blockShapeZ1 * 16f);

		u01 = u11;
		u10 = u00;
		v01 = v00;
		v10 = v11;
		u01 = u00;
		u10 = u11;
		v00 = v11;
		v11 = v01;
	} else if (eastFlip == FLIP_CW) {
		// reshape
		u00 = tex.inverseOffsetWidth(mCurrentShape.max.y);	//tex.getU(SharedConstants.WORLD_RESOLUTION - blockShapeY1 * 16f);
		v00 = tex.offsetHeight(mCurrentShape.max.z);//tex.getV(blockShapeZ1 * 16f);
		u11 = tex.inverseOffsetWidth(mCurrentShape.min.y);	//tex.getU(SharedConstants.WORLD_RESOLUTION - blockShapeY0 * 16f);
		v11 = tex.offsetHeight(mCurrentShape.min.z);//tex.getV(blockShapeZ0 * 16f);

		// rotate
		u01 = u11;
		u10 = u00;
		v01 = v00;
		v10 = v11;
		u00 = u01;
		u11 = u10;
		v01 = v11;
		v10 = v00;
	} else if (eastFlip == FLIP_180) {
		u00 = tex.inverseOffsetWidth(mCurrentShape.min.z);	// tex.getU(SharedConstants.WORLD_RESOLUTION - blockShapeZ0 * 16f);
		u11 = tex.inverseOffsetWidth(mCurrentShape.max.z);	// tex.getU(SharedConstants.WORLD_RESOLUTION - blockShapeZ1 * 16f);
		v00 = tex.offsetHeight(mCurrentShape.max.y);// tex.getV(blockShapeY1 * 16f);
		v11 = tex.offsetHeight(mCurrentShape.min.y);// tex.getV(blockShapeY0 * 16f);

		u01 = u11;
		u10 = u00;
		v01 = v00;
		v10 = v11;
	}

	Vec3 a = p + mCurrentShape.min, b = p + mCurrentShape.max;

	if (mUseNormals) {
		mTessellator.normal(Vec3::UNIT_X);
	}

	if (mApplyAmbientOcclusion) {

		std::array<Color, 4> corners = { mAoColors[1], mAoColors[2], mAoColors[0 + 4], mAoColors[3 + 4] };

		mTessellator.color(Color::bilinear(corners, mCurrentShape.max.z, mCurrentShape.min.y));
		_tex1(toUV(mTc[2]));
		mTessellator.vertexUV(b.x, a.y, b.z, u10, v10);

		mTessellator.color(Color::bilinear(corners, mCurrentShape.min.z, mCurrentShape.min.y));
		_tex1(toUV(mTc[1]));
		mTessellator.vertexUV(b.x, a.y, a.z, u11, v11);

		mTessellator.color(Color::bilinear(corners, mCurrentShape.min.z, mCurrentShape.max.y));
		_tex1(toUV(mTc[0]));
		mTessellator.vertexUV(b.x, b.y, a.z, u01, v01);

		mTessellator.color(Color::bilinear(corners, mCurrentShape.max.z, mCurrentShape.max.y));
		_tex1(toUV(mTc[3]));
		mTessellator.vertexUV(b.x, b.y, b.z, u00, v00);
	} else {
		mTessellator.vertexUV(b.x, a.y, b.z, u10, v10);
		mTessellator.vertexUV(b.x, a.y, a.z, u11, v11);
		mTessellator.vertexUV(b.x, b.y, a.z, u01, v01);
		mTessellator.vertexUV(b.x, b.y, b.z, u00, v00);
	}
}

void BlockTessellator::renderAll(const Block& block, const Vec3& p, const TextureUVCoordinateSet& tex)
{
	renderFaceDown(block, p, tex);
	renderFaceUp(block, p, tex);
	renderNorth(block, p, tex);
	renderSouth(block, p, tex);
	renderWest(block, p, tex);
	renderEast(block, p, tex);
}


void BlockTessellator::appendTessellatedBlock(const FullBlock& fullBlock) {
	const Block& block = *Block::mBlocks[fullBlock.id];
	const BlockGraphics& blockGraphics = *BlockGraphics::mBlocks[fullBlock.id];

	memset(mFlipFace, 0, sizeof(mFlipFace));

	//mUseNormals is used inside renderFace* to assign the normal if needed
	mUseNormals = true;
	mApplyAmbientOcclusion = false;

	auto blockShape = BlockGraphics::mBlocks[block.getId()]->getBlockShape();
	mCurrentShape = block.getVisualShape(fullBlock.data, mCurrentShape);
	int dataVariant = block.getVariant(fullBlock.data);

	if (block.isType(Block::mWoodButton) || block.isType(Block::mStoneButton)) {
		mTessellator.begin(6 * 4);

		renderFaceDown(block, Vec3::ZERO, blockGraphics.getCarriedTexture(Facing::DOWN, dataVariant));
		renderFaceUp(block, Vec3::ZERO, blockGraphics.getCarriedTexture(Facing::UP, dataVariant));
		renderNorth(block, Vec3::ZERO, blockGraphics.getCarriedTexture(Facing::NORTH, dataVariant));
		renderSouth(block, Vec3::ZERO, blockGraphics.getCarriedTexture(Facing::SOUTH, dataVariant));
		renderWest(block, Vec3::ZERO, blockGraphics.getCarriedTexture(Facing::WEST, dataVariant));
		renderEast(block, Vec3::ZERO, blockGraphics.getCarriedTexture(Facing::EAST, dataVariant));

		mTessellator.setOffset(Vec3::ZERO);
	}
	else if (	blockShape == BlockShape::BLOCK || 
				blockShape == BlockShape::TREE || 
				blockShape == BlockShape::BLOCK_HALF || 
				blockShape == BlockShape::TOP_SNOW || 
				blockShape == BlockShape::PISTON
		) 
	{
		if (blockShape != BlockShape::TOP_SNOW) {
			AABB centeredShape = mCurrentShape;
			centeredShape.centerAt(Vec3::ZERO);
			mTessellator.setOffset(centeredShape.min);
		}
		else {
			mTessellator.setOffset(-0.5f, -0.5f, -0.5f);
		}
		mTessellator.begin(6 * 4);

		renderFaceDown(block, Vec3::ZERO, blockGraphics.getCarriedTexture(Facing::DOWN, dataVariant));
		renderFaceUp(block, Vec3::ZERO, blockGraphics.getCarriedTexture(Facing::UP, dataVariant));
		renderNorth(block, Vec3::ZERO, blockGraphics.getCarriedTexture(Facing::NORTH, dataVariant));
		renderSouth(block, Vec3::ZERO, blockGraphics.getCarriedTexture(Facing::SOUTH, dataVariant));
		renderWest(block, Vec3::ZERO, blockGraphics.getCarriedTexture(Facing::WEST, dataVariant));
		renderEast(block, Vec3::ZERO, blockGraphics.getCarriedTexture(Facing::EAST, dataVariant));
		
		mTessellator.setOffset(Vec3::ZERO);

	} else if (blockShape == BlockShape::SLIME_BLOCK) {

		AABB centeredShape = mCurrentShape;
		centeredShape.centerAt(Vec3::ZERO);
		mTessellator.setOffset(centeredShape.min);

		AABB outerShape = mCurrentShape;
		const float innerShape = 3.0f / 16.0f;
		mCurrentShape.set(Vec3(innerShape), Vec3::ONE - Vec3(innerShape));

		mTessellator.color(1.0f, 1.0f, 1.0f, 1.0f);

		mTessellator.begin(12 * 4);

		renderFaceDown(block, Vec3::ZERO, blockGraphics.getCarriedTexture(Facing::DOWN, dataVariant));
		renderFaceUp(block, Vec3::ZERO, blockGraphics.getCarriedTexture(Facing::UP, dataVariant));
		renderNorth(block, Vec3::ZERO, blockGraphics.getCarriedTexture(Facing::NORTH, dataVariant));
		renderSouth(block, Vec3::ZERO, blockGraphics.getCarriedTexture(Facing::SOUTH, dataVariant));
		renderWest(block, Vec3::ZERO, blockGraphics.getCarriedTexture(Facing::WEST, dataVariant));
		renderEast(block, Vec3::ZERO, blockGraphics.getCarriedTexture(Facing::EAST, dataVariant));

		mCurrentShape = outerShape;

		mTessellator.color(1.0f, 1.0f, 1.0f, 0.25f);

		renderFaceDown(block, Vec3::ZERO, blockGraphics.getCarriedTexture(Facing::DOWN, dataVariant));
		renderFaceUp(block, Vec3::ZERO, blockGraphics.getCarriedTexture(Facing::UP, dataVariant));
		renderNorth(block, Vec3::ZERO, blockGraphics.getCarriedTexture(Facing::NORTH, dataVariant));
		renderSouth(block, Vec3::ZERO, blockGraphics.getCarriedTexture(Facing::SOUTH, dataVariant));
		renderWest(block, Vec3::ZERO, blockGraphics.getCarriedTexture(Facing::WEST, dataVariant));
		renderEast(block, Vec3::ZERO, blockGraphics.getCarriedTexture(Facing::EAST, dataVariant));

		mTessellator.setOffset(Vec3::ZERO);

	} else if (blockShape == BlockShape::CROSS_TEXTURE) {
		mTessellator.begin(4 * 2);
		tessellateCrossTexture(block, fullBlock.data, Vec3(-0.5f, -0.5f, -0.5f), true);
	} else if (blockShape == BlockShape::CROSS_TEXTURE_POLY) {
		const TextureUVCoordinateSet& tex = blockGraphics.getTexture(Facing::DOWN, dataVariant);
		auto& vec = BlockTextureTessellator::getTessellatedTexturesInfo(tex.sourceFileLocation);
		mTessellator.begin(vec.size() * 4 * 2);
		tessellateCrossPolyTexture(tex, { -0.5f, -0.5f, -0.5f }, true, block);
	} else if (blockShape == BlockShape::STEM) {
		mTessellator.begin(4 * 2);
		tessellateStemTexture(block, fullBlock.data, mCurrentShape.max.y, -0.5f, -0.5f, -0.5f);
	} else if (blockShape == BlockShape::CACTUS) {
		mTessellator.setOffset(-0.5f, -0.5f, -0.5f);
		float s = 1 / 16.0f;
		mTessellator.begin(6 * 4);
		renderFaceDown(block, Vec3::ZERO, blockGraphics.getTexture(Facing::DOWN));
		renderFaceUp(block, Vec3::ZERO, blockGraphics.getTexture(Facing::UP));
		mTessellator.addOffset(0, 0, s);
		renderNorth(block, Vec3::ZERO, blockGraphics.getTexture(Facing::NORTH));
		mTessellator.addOffset(0, 0, -s);
		mTessellator.addOffset(0, 0, -s);
		renderSouth(block, Vec3::ZERO, blockGraphics.getTexture(Facing::SOUTH));
		mTessellator.addOffset(0, 0, s);
		mTessellator.addOffset(s, 0, 0);
		renderWest(block, Vec3::ZERO, blockGraphics.getTexture(Facing::WEST));
		mTessellator.addOffset(-s, 0, 0);
		mTessellator.addOffset(-s, 0, 0);
		renderEast(block, Vec3::ZERO, blockGraphics.getTexture(Facing::EAST));
		mTessellator.addOffset(s, 0, 0);

		mTessellator.setOffset(0, 0, 0);//0.5f, 0.5f, 0.5f);
	} else if (blockShape == BlockShape::ROWS) {
		mTessellator.begin(0);
		mTessellator.normal(0, -1, 0);
		tessellateRowTexture(block, fullBlock.data, -0.5f, -0.5f, -0.5f);
		//}  else if (blockShape == BlockShape::TORCH) {
		////	mTessellator.begin();
		////	mTessellator.normal(0, -1, 0);
		////	tessellateTorch(block, -0.5f, -0.5f, -0.5f, 0, 0);
		////	t.end();
	} else if (blockShape == BlockShape::CHEST) {
		DEBUG_FAIL("We don't want to tesselate the chest in this way.");

	} else if (blockShape == BlockShape::STAIRS) {
		mTessellator.addOffset(-0.5f, -0.5f, -0.5f);
		mTessellator.begin(6 * 4 * 2);

		for (int i = 0; i < 2; i++) {
			if (i == 0) {
				mCurrentShape.set(0, 0, 0, 1, 1, 0.5f);
			}
			if (i == 1) {
				mCurrentShape.set(0, 0, 0.5f, 1, 0.5f, 1);
			}
			
			renderFaceDown(block, Vec3::ZERO, blockGraphics.getTexture(Facing::DOWN));
			renderFaceUp(block, Vec3::ZERO, blockGraphics.getTexture(Facing::UP));
			renderNorth(block, Vec3::ZERO, blockGraphics.getTexture(Facing::NORTH));
			renderSouth(block, Vec3::ZERO, blockGraphics.getTexture(Facing::SOUTH));
			renderWest(block, Vec3::ZERO, blockGraphics.getTexture(Facing::WEST));
			renderEast(block, Vec3::ZERO, blockGraphics.getTexture(Facing::EAST));
		}

		mTessellator.addOffset(0.5f, 0.5f, 0.5f);
	}
	else if (blockShape == BlockShape::ANVIL) {
		mRenderingGUI = true;
		mTessellator.addOffset(-0.5f, -0.5f, -0.5f);
		tessellateAnvilInWorld(static_cast<const AnvilBlock&>(block), BlockPos::ZERO, fullBlock.data, true);
		mTessellator.addOffset(0.5f, 0.5f, 0.5f);
		mRenderingGUI = false;
	} else if (blockShape == BlockShape::STRUCTURE_VOID) {
		mRenderingGUI = true;
		mTessellator.addOffset(-0.5f, -0.5f, -0.5f);
		tessellateStructureVoidInWorld(block, BlockPos::ZERO, fullBlock.data, true);
		mTessellator.addOffset(0.5f, 0.5f, 0.5f);
		mTessellator.resetScale();
		mRenderingGUI = false;
	} else if (blockShape == BlockShape::FENCE) {
		mTessellator.addOffset(-0.5f, -0.5f, -0.5f);
		mTessellator.begin(6 * 4 * 4);

		for (int i = 0; i < 6; i++) {
			float w = 2 / 16.0f;
			if (i == 0) {
				mCurrentShape.set(0.5f - w, 0, 0, 0.5f + w, 1, w * 2);
			}
			if (i == 1) {
				mCurrentShape.set(0.5f - w, 0, 1 - w * 2, 0.5f + w, 1, 1);
			}
			w = 1 / 16.0f;
			if (i == 2) {
				mCurrentShape.set(0.5f - w, 1 - w * 3, -w * 2, 0.5f + w, 1 - w, 0.5f);
			}
			if (i == 3) {
				mCurrentShape.set(0.5f - w, 0.5f - w * 3, -w * 2, 0.5f + w, 0.5f - w, 0.5f);
			}
			if (i == 4) {
				mCurrentShape.set(0.5f - w, 1 - w * 3, 0.5f, 0.5f + w, 1 - w, 1 + w * 2);
			}
			if (i == 5) {
				mCurrentShape.set(0.5f - w, 0.5f - w * 3, 0.5f, 0.5f + w, 0.5f - w, 1 + w * 2);
			}

			renderFaceDown(block, Vec3::ZERO, blockGraphics.getTexture(Facing::DOWN, dataVariant));
			renderFaceUp(block, Vec3::ZERO, blockGraphics.getTexture(Facing::UP, dataVariant));
			renderNorth(block, Vec3::ZERO, blockGraphics.getTexture(Facing::NORTH, dataVariant));
			renderSouth(block, Vec3::ZERO, blockGraphics.getTexture(Facing::SOUTH, dataVariant));
			renderWest(block, Vec3::ZERO, blockGraphics.getTexture(Facing::WEST, dataVariant));
			renderEast(block, Vec3::ZERO, blockGraphics.getTexture(Facing::EAST, dataVariant));
		}

		mTessellator.addOffset(0.5f, 0.5f, 0.5f);
	} else if (blockShape == BlockShape::FENCE_GATE) {
		mTessellator.addOffset(-0.5f, -0.5f, -0.5f);
		mTessellator.begin(6 * 4 * 3);

		for (int i = 0; i < 3; i++) {
			float w = 1 / 16.0f;
			if (i == 0) {
				mCurrentShape.set(0.5f - w, .3f, 0, 0.5f + w, 1, w * 2);
			}
			if (i == 1) {
				mCurrentShape.set(0.5f - w, .3f, 1 - w * 2, 0.5f + w, 1, 1);
			}
			if (i == 2) {
				mCurrentShape.set(0.5f - w, .5f, w * 2, 0.5f + w, 1 - w, 1 - w * 2);
			}

			renderFaceUp(block, Vec3::ZERO, blockGraphics.getTexture(Facing::UP));
			renderFaceDown(block, Vec3::ZERO, blockGraphics.getTexture(Facing::DOWN));
			renderNorth(block, Vec3::ZERO, blockGraphics.getTexture(Facing::NORTH));
			renderSouth(block, Vec3::ZERO, blockGraphics.getTexture(Facing::SOUTH));
			renderWest(block, Vec3::ZERO, blockGraphics.getTexture(Facing::WEST));
			renderEast(block, Vec3::ZERO, blockGraphics.getTexture(Facing::EAST));
		}

		mTessellator.addOffset(0.5f, 0.5f, 0.5f);
	} else if(blockShape == BlockShape::PORTAL_FRAME) {
		mRenderingGUI = true;
		mRenderingData = fullBlock.data;
		int nrBoxes = EndPortalFrameBlock::hasEye(fullBlock.data) ? 2 : 1;
		mTessellator.setOffset(-0.5f, -0.5f, -0.5f);

		mTessellator.begin( mce::PrimitiveMode::QuadList, 6 * 4 * nrBoxes );
		tessellateEndPortalFrameInWorld(static_cast<const EndPortalFrameBlock&>(block), BlockPos::ZERO, fullBlock.data);

		mRenderingGUI = false;

		mTessellator.setOffset(Vec3::ZERO);
	} else if (blockShape == BlockShape::WALL) {
		mTessellator.begin(6 * 4 * 4);

		for (int i = 0; i < 4; i++) {
			if (i == 0) {
				mCurrentShape.set(.5f - WallBlock::POST_WIDTH,
					0,
					0.5f + .5f - WallBlock::POST_WIDTH,
					.5f + WallBlock::POST_WIDTH,
					WallBlock::POST_HEIGHT,
					0.5f + .5f + WallBlock::POST_WIDTH);
			} else if (i == 1) {
				mCurrentShape.set(.5f - WallBlock::WALL_WIDTH,
					0,
					0.5f + 0,
					.5f + WallBlock::WALL_WIDTH,
					WallBlock::WALL_HEIGHT,
					0.5f + .5f - WallBlock::POST_WIDTH);
			} else if (i == 2) {
				mCurrentShape.set(.5f - WallBlock::WALL_WIDTH,
					0,
					-0.5f + .5f + WallBlock::POST_WIDTH,
					.5f + WallBlock::WALL_WIDTH,
					WallBlock::WALL_HEIGHT,
					-0.5f + 1);
			} else{
				mCurrentShape.set(.5f - WallBlock::POST_WIDTH,
					0,
					-0.5f + .5f - WallBlock::POST_WIDTH,
					.5f + WallBlock::POST_WIDTH,
					WallBlock::POST_HEIGHT,
					-0.5f + .5f + WallBlock::POST_WIDTH);
			}

			_renderAllFaces(block, Vec3::ZERO, dataVariant);
		}
	} else if (blockShape == BlockShape::HOPPER) {
		mTessellator.addOffset(-0.5f, -0.5f, -0.5f);
		tessellateHopperInWorld(block, BlockPos::ZERO, 0, true);
		mTessellator.addOffset(0.5f, 0.5f, 0.5f);
	}
	else if (blockShape == BlockShape::BEACON) {
		auto& texBase = _getTexture(*Block::mObsidian, Facing::UP, fullBlock.data);// block.getTextureNum(0);
		auto& texCore = _getTexture(block, Facing::UP, fullBlock.data);//block.getTextureNum(1);
		auto& texShell = _getTexture(*Block::mGlass, Facing::UP, fullBlock.data);//block.getTextureNum(2);

		mTessellator.color(1.0f, 1.0f, 1.0f);

		// beacon base
		const float fDiv = 1.0f / 16.0f;
		mCurrentShape.set(Vec3(2.0f * fDiv, 0.1f * fDiv, 2.0f * fDiv), Vec3(14.0f * fDiv, 3.0f * fDiv, 14.0f * fDiv));
		renderAll(block, Vec3::ZERO, texBase);

		// beacon core
		mCurrentShape.set(Vec3(3.0f * fDiv, 3.0f * fDiv, 3.0f * fDiv), Vec3(13.0f * fDiv, 14.0f * fDiv, 13.0f * fDiv));
		renderAll(block, Vec3::ZERO, texCore);

		// beacon outer shell
		mCurrentShape.set(Vec3::ZERO, Vec3::ONE);
		renderAll(block, Vec3::ZERO, texShell);
	}
	else if (blockShape == BlockShape::CHORUS_PLANT || blockShape == BlockShape::CHORUS_FLOWER) {
		mTessellator.addOffset(-0.5f, -0.5f, -0.5f);
		tessellateChorusFlowerInWorld(block, Vec3::ZERO, dataVariant, true);
		// Translate the offset back to the center, remember, always put your weights away at the gym
		mTessellator.addOffset(0.5f, 0.5f, 0.5f);
	}
	else if (blockShape == BlockShape::DRAGON_EGG) {
		mTessellator.addOffset(-0.5f, -0.5f, -0.5f);
		tessellateDragonEgg(block, BlockPos::ZERO, 0, true);
		mTessellator.addOffset(0.5f, 0.5f, 0.5f);
	}
	else if (blockShape == BlockShape::END_ROD) {
		mTessellator.addOffset(-0.5f, -0.5f, -0.5f);
		tessellateEndRodAppended(block, blockGraphics);
		mTessellator.addOffset(0.5f, 0.5f, 0.5f);
	}

	mUseNormals = false;
}

// mce::Mesh& BlockTessellator::getMeshForBlockInWorld( const FullBlock& block, BlockPos& blockPos, BlockRenderLayer layer, const void* owner) {
// 	auto key = BlockTessellatorBlockInWorld{ block, layer,blockPos, owner };
// 
// 	auto elem = mInWorldBlockMeshes.find(key);
// 	if (elem != mInWorldBlockMeshes.end()) {
// 		return elem->second;
// 	}
// 
// 	//flush the cache once it's "200" big whatever that means
// 	if (mInWorldBlockMeshes.size() == 200) {
// 		mInWorldBlockMeshes.clear();
// 	}
// 
// 	mUseNormals = true;
// 	mUseOccluder = true;
// 	_tmpUseRegion = false;	// TODO: remove when BlockModels!
// 	const Block* blockType = Block::mBlocks[block.id];
// 	mCurrentShape = blockType->getVisualShape(block.data, mCurrentShape);
// 
// 	//auto base = Color::fromARGB(blockType->getColor(block.data));
// 	Color base = Color::fromARGB(blockType->getColor(*mRegion, blockPos));
// 
// 	// light values when not in UI mode
// 	resetCache(blockPos, *mRegion);
// 
// 	setRenderLayer(layer);
// 
// 	mTessellator.beginOverride();
// 	mTessellator.color(base);
// 
// 	if(mUseOccluder) {
// 		tessellateInWorld(*blockType, blockPos, block.data);
// 	}
// 
// 	mUseOccluder = false;
// 	mUseNormals = false;
// 	_tmpUseRegion = true;
// 
// 	return mInWorldBlockMeshes[key] = mTessellator.endOverride();
// }
// 
// mce::Mesh& BlockTessellator::getMeshForBlock( const FullBlock& block ) {
// 	int ID = (block.id << 8) | (block.data & 0xf);
// 
// 	auto elem = mBlockMeshes.find(ID);
// 	if (elem != mBlockMeshes.end()) {
// 		return elem->second;
// 	} else {
// 		mTessellator.beginOverride();
// 
// 		appendTessellatedBlock(block);
// 
// 		return mBlockMeshes[ID] = mTessellator.endOverride();
// 	}
// }

bool BlockTessellator::canRender(BlockShape blockShape) {
	return	blockShape == BlockShape::ANVIL ||
		blockShape == BlockShape::BEACON ||
		blockShape == BlockShape::BLOCK ||
		blockShape == BlockShape::BLOCK_HALF ||
		blockShape == BlockShape::TOP_SNOW ||
		blockShape == BlockShape::CACTUS ||
		blockShape == BlockShape::STAIRS ||
		blockShape == BlockShape::FENCE ||
		blockShape == BlockShape::FENCE_GATE ||
		blockShape == BlockShape::WALL ||
		blockShape == BlockShape::CHEST ||
		blockShape == BlockShape::TREE ||
		blockShape == BlockShape::PORTAL_FRAME ||
		blockShape == BlockShape::SLIME_BLOCK ||
		blockShape == BlockShape::PISTON ||
		blockShape == BlockShape::CHORUS_PLANT ||
		blockShape == BlockShape::CHORUS_FLOWER ||
		blockShape == BlockShape::DRAGON_EGG ||
		blockShape == BlockShape::END_ROD ||
		blockShape == BlockShape::STRUCTURE_VOID ||
		blockShape == BlockShape::SKULL;
}

// void BlockTessellator::renderGuiBlock(const FullBlock& fullBlock, const mce::TexturePtr& texture, float lightMultiplyer /*= 1.0f*/, float alphaMultiplyer /*= 1.0f*/) {
// 	mRenderingGUI = true;
// 	bool originalSmoothLight = mSmoothLighting;
// 	mSmoothLighting = false;
// 	mRenderingData = fullBlock.data;
// 	const Block& block = *Block::mBlocks[fullBlock.id];
// 	const BlockGraphics& blockGraphics = *BlockGraphics::mBlocks[fullBlock.id];
// 	Tessellator& t = Tessellator::instance;
// 	bool _mUseNormals = mUseNormals;
// 	mUseNormals = false;
// 	mCurrentShape = block.getVisualShape(fullBlock.data, mCurrentShape);
// 
// 	BlockShape blockShape = BlockGraphics::mBlocks[fullBlock.id]->getBlockShape();
// 	int dataVariant = block.getVariant(fullBlock.data);
// 
// 	bool e = Block::mLightEmission[block.mID] > 0;
// 	const float c10 = e ? 1.f : 0.5f;
// 	const float c2 = e ? 1.f : 0.73f;
// 
// 	if (blockShape == BlockShape::BLOCK ||
// 		blockShape == BlockShape::TREE ||
// 		blockShape == BlockShape::CHEST ||
// 		blockShape == BlockShape::BLOCK_HALF ||
// 		blockShape == BlockShape::TOP_SNOW ||
// 		blockShape == BlockShape::PISTON ||
// 		blockShape == BlockShape::SKULL) {
// 		
// 		float r = 1 * lightMultiplyer;
// 		float g = 1 * lightMultiplyer;
// 		float b = 1 * lightMultiplyer;
// 
// 		mTessellator.begin(mce::PrimitiveMode::QuadList, 6 * 4 /*6*4*/);
// 		mTessellator.color(r, g, b, alphaMultiplyer);
// 		renderFaceUp(block, Vec3::ZERO, blockGraphics.getCarriedTexture(Facing::UP, dataVariant));
// 
// 		mTessellator.color(c10 * lightMultiplyer, c10 * lightMultiplyer, c10 * lightMultiplyer, alphaMultiplyer);
// 		renderSouth(block, Vec3::ZERO, blockGraphics.getCarriedTexture(Facing::SOUTH, dataVariant));
// 
// 		mTessellator.color(c2 * lightMultiplyer, c2 * lightMultiplyer, c2 * lightMultiplyer, alphaMultiplyer);
// 		renderWest(block, Vec3::ZERO, blockGraphics.getCarriedTexture(Facing::WEST, dataVariant));
// 		t.draw(mItemMat, texture);
// 	} else if (blockShape == BlockShape::SLIME_BLOCK) {
// 		float r = 1 * lightMultiplyer;
// 		float g = 1 * lightMultiplyer;
// 		float b = 1 * lightMultiplyer;
// 
// 		// Boy I can't wait until block models come in :(
// 		AABB prevShape = mCurrentShape;
// 
// 		const float innerShape = 3.0f / 16.0f;
// 		mCurrentShape.set(Vec3(innerShape), Vec3::ONE - Vec3(innerShape));
// 
// 		mTessellator.begin(mce::PrimitiveMode::QuadList, 6 * 4 /*6*4*/);
// 		mTessellator.color(r, g, b, alphaMultiplyer);
// 		renderFaceUp(block, Vec3::ZERO, blockGraphics.getCarriedTexture(Facing::UP, dataVariant));
// 
// 		mTessellator.color(c10 * lightMultiplyer, c10 * lightMultiplyer, c10 * lightMultiplyer, alphaMultiplyer);
// 		renderSouth(block, Vec3::ZERO, blockGraphics.getCarriedTexture(Facing::SOUTH, dataVariant));
// 
// 		mTessellator.color(c2 * lightMultiplyer, c2 * lightMultiplyer, c2 * lightMultiplyer, alphaMultiplyer);
// 		renderWest(block, Vec3::ZERO, blockGraphics.getCarriedTexture(Facing::WEST, dataVariant));
// 
// 		t.draw(mItemMat, texture);
// 
// 		mCurrentShape = prevShape;
// 
// 		mTessellator.begin(mce::PrimitiveMode::QuadList, 6 * 4 /*6*4*/);
// 		mTessellator.color(r, g, b, 0.25f*alphaMultiplyer);
// 		renderFaceUp(block, Vec3::ZERO, blockGraphics.getCarriedTexture(Facing::UP, dataVariant));
// 
// 		mTessellator.color(c10 * lightMultiplyer, c10 * lightMultiplyer, c10 * lightMultiplyer, 0.25f*alphaMultiplyer);
// 		renderSouth(block, Vec3::ZERO, blockGraphics.getCarriedTexture(Facing::SOUTH, dataVariant));
// 
// 		mTessellator.color(c2 * lightMultiplyer, c2 * lightMultiplyer, c2 * lightMultiplyer, 0.25f*alphaMultiplyer);
// 		renderWest(block, Vec3::ZERO, blockGraphics.getCarriedTexture(Facing::WEST, dataVariant));
// 		t.draw(mItemMat, texture);
// 	} else if(blockShape == BlockShape::BEACON) {
// 		mTessellator.begin(mce::PrimitiveMode::QuadList, 6 * 4 * 3);
// 		tessellateBeacon(block, lightMultiplyer);
// 		mTessellator.draw(mItemMat, texture);
// 	} else if(blockShape == BlockShape::CROSS_TEXTURE) {
// 		mTessellator.begin(4 * 2);
// 		//mTessellator.normal(0, -1, 0);
// 		mTessellator.color(lightMultiplyer, lightMultiplyer, lightMultiplyer, alphaMultiplyer);
// 		tessellateCrossTexture(block, fullBlock.data, Vec3(-0.5f, -0.5f, -0.5f), true);
// 		//t.end();
// 		t.draw(mItemMat, texture);
// 	} else if(blockShape == BlockShape::CACTUS) {
// 		mTessellator.begin(6 * 4);
// 		
// 		mTessellator.color(lightMultiplyer, lightMultiplyer, lightMultiplyer, alphaMultiplyer);
// 		renderFaceDown(block, Vec3::ZERO, blockGraphics.getTexture(Facing::DOWN));
// 		renderFaceUp(block, Vec3::ZERO, blockGraphics.getTexture(Facing::UP));
// 
// 		Vec3 offset_z(0, 0, 1/16.0f);
// 		mTessellator.color(c10 * lightMultiplyer, c10 * lightMultiplyer, c10 * lightMultiplyer, alphaMultiplyer);
// 		renderNorth(block, offset_z, blockGraphics.getTexture(Facing::NORTH));
// 		renderSouth(block, -offset_z, blockGraphics.getTexture(Facing::SOUTH));
// 
// 		Vec3 offset_x(1/16.f, 0, 0);
// 		mTessellator.color(c2 * lightMultiplyer, c2 * lightMultiplyer, c2 * lightMultiplyer, alphaMultiplyer);
// 		renderWest(block, offset_x, blockGraphics.getTexture(Facing::WEST));
// 		renderEast(block, -offset_x, blockGraphics.getTexture(Facing::EAST));
// 
// 		t.draw(mItemMat, texture);
// 	} else if(blockShape == BlockShape::STAIRS) {
// 		mTessellator.begin(6 * 4 * 2);
// 
// 		for (int i = 0; i < 2; i++) {
// 			if (i == 0) {
// 				mCurrentShape.set(0, 0, 0, 1, 1, 0.5f);
// 			}
// 
// 			if (i == 1) {
// 				mCurrentShape.set(0, 0, 0.5f, 1, 0.5f, 1);
// 			}
// 
// 			mTessellator.color(lightMultiplyer, lightMultiplyer, lightMultiplyer, alphaMultiplyer);
// 			renderFaceDown(block, Vec3::ZERO, blockGraphics.getTexture(Facing::DOWN));
// 			renderFaceUp(block, Vec3::ZERO, blockGraphics.getTexture(Facing::UP));
// 
// 			mTessellator.color(c10 * lightMultiplyer, c10 * lightMultiplyer, c10 * lightMultiplyer, alphaMultiplyer);
// 			renderNorth(block, Vec3::ZERO, blockGraphics.getTexture(Facing::NORTH));
// 			renderSouth(block, Vec3::ZERO, blockGraphics.getTexture(Facing::SOUTH));
// 
// 			mTessellator.color(c2 * lightMultiplyer, c2 * lightMultiplyer, c2 * lightMultiplyer, alphaMultiplyer);
// 			renderWest(block, Vec3::ZERO, blockGraphics.getTexture(Facing::WEST));
// 			renderEast(block, Vec3::ZERO, blockGraphics.getTexture(Facing::EAST));
// 		}
// 		t.draw(mItemMat, texture);
// 	} else if(blockShape == BlockShape::FENCE) {
// 		mTessellator.begin(6 * 4 * 4);
// 
// 		for (int i = 0; i < 6; i++) {
// 			float w = 2 / 16.0f;
// 			if (i == 0) {
// 				mCurrentShape.set(0.5f - w, 0, 0, 0.5f + w, 1, w * 2);
// 			}
// 			if (i == 1) {
// 				mCurrentShape.set(0.5f - w, 0, 1 - w * 2, 0.5f + w, 1, 1);
// 			}
// 			w = 1 / 16.0f;
// 			if (i == 2) {
// 				mCurrentShape.set(0.5f - w, 1 - w * 3, -w * 2, 0.5f + w, 1 - w, 0.5f);
// 			}
// 			if (i == 3) {
// 				mCurrentShape.set(0.5f - w, 0.5f - w * 3, -w * 2, 0.5f + w, 0.5f - w, 0.5f);
// 			}
// 			if (i == 4) {
// 				mCurrentShape.set(0.5f - w, 1 - w * 3, 0.5f, 0.5f + w, 1 - w, 1 + w * 2);
// 			}
// 			if (i == 5) {
// 				mCurrentShape.set(0.5f - w, 0.5f - w * 3, 0.5f, 0.5f + w, 0.5f - w, 1 + w * 2);
// 			}
// 
// 			mTessellator.color(lightMultiplyer, lightMultiplyer, lightMultiplyer, alphaMultiplyer);
// 
// 			renderFaceDown(block, Vec3::ZERO, blockGraphics.getTexture(Facing::DOWN, dataVariant));
// 			renderFaceUp(block, Vec3::ZERO, blockGraphics.getTexture(Facing::UP, dataVariant));
// 
// 			mTessellator.color(c10 * lightMultiplyer, c10 * lightMultiplyer, c10 * lightMultiplyer, alphaMultiplyer);
// 			renderNorth(block, Vec3::ZERO, blockGraphics.getTexture(Facing::NORTH, dataVariant));
// 			renderSouth(block, Vec3::ZERO, blockGraphics.getTexture(Facing::SOUTH, dataVariant));
// 
// 			mTessellator.color(c2 * lightMultiplyer, c2 * lightMultiplyer, c2 * lightMultiplyer, alphaMultiplyer);
// 			renderWest(block, Vec3::ZERO, blockGraphics.getTexture(Facing::WEST, dataVariant));
// 			renderEast(block, Vec3::ZERO, blockGraphics.getTexture(Facing::EAST, dataVariant));
// 		}
// 		t.draw(mItemMat, texture);
// 	} else if(blockShape == BlockShape::FENCE_GATE) {
// 		mTessellator.begin(6 * 4 * 3);
// 
// 		for (int i = 0; i < 3; i++) {
// 			float w = 1 / 16.0f;
// 			if (i == 0) {
// 				mCurrentShape.set(0.5f - w, .3f, 0, 0.5f + w, 1, w * 2);
// 			}
// 
// 			if (i == 1) {
// 				mCurrentShape.set(0.5f - w, .3f, 1 - w * 2, 0.5f + w, 1, 1);
// 			}
// 
// 			w = 1 / 16.0f;
// 			if (i == 2) {
// 				mCurrentShape.set(0.5f - w, .5f, 0, 0.5f + w, 1 - w, 1);
// 			}
// 
// 			mTessellator.color(lightMultiplyer, lightMultiplyer, lightMultiplyer, alphaMultiplyer);
// 			renderFaceUp(block, Vec3::ZERO, blockGraphics.getTexture(Facing::UP));
// 			renderFaceDown(block, Vec3::ZERO, blockGraphics.getTexture(Facing::DOWN));
// 
// 			mTessellator.color(c10 * lightMultiplyer, c10 * lightMultiplyer, c10 * lightMultiplyer, alphaMultiplyer);
// 			renderNorth(block, Vec3::ZERO, blockGraphics.getTexture(Facing::NORTH));
// 			renderSouth(block, Vec3::ZERO, blockGraphics.getTexture(Facing::SOUTH));
// 
// 			mTessellator.color(c2 * lightMultiplyer, c2 * lightMultiplyer, c2 * lightMultiplyer, alphaMultiplyer);
// 			renderWest(block, Vec3::ZERO, blockGraphics.getTexture(Facing::WEST));
// 			renderEast(block, Vec3::ZERO, blockGraphics.getTexture(Facing::EAST));
// 		}
// 		t.draw(mItemMat, texture);
// 	} else if(blockShape == BlockShape::WALL) {
// 		mTessellator.begin(6 * 4 * 4);
// 
// 		for(auto i : range(4)) {
// 			if(i == 0) {
// 				mCurrentShape.set(0.5f + .5f - WallBlock::POST_WIDTH, 0, .5f - WallBlock::POST_WIDTH, 0.5f + .5f + WallBlock::POST_WIDTH, WallBlock::POST_HEIGHT, .5f + WallBlock::POST_WIDTH);
// 			} else if(i == 1) {
// 				mCurrentShape.set(0.5f + 0, 0, .5f - WallBlock::WALL_WIDTH, 0.5f + .5f - WallBlock::POST_WIDTH, WallBlock::WALL_HEIGHT, .5f + WallBlock::WALL_WIDTH);
// 			} else if(i == 2) {
// 				mCurrentShape.set(-0.5f + .5f + WallBlock::POST_WIDTH, 0, .5f - WallBlock::WALL_WIDTH, -0.5f + 1, WallBlock::WALL_HEIGHT, .5f + WallBlock::WALL_WIDTH);
// 			} else{
// 				mCurrentShape.set(-0.5f + .5f - WallBlock::POST_WIDTH, 0, .5f - WallBlock::POST_WIDTH, -0.5f + .5f + WallBlock::POST_WIDTH, WallBlock::POST_HEIGHT, .5f + WallBlock::POST_WIDTH);
// 			}
// 
// 			mTessellator.color(lightMultiplyer, lightMultiplyer, lightMultiplyer, alphaMultiplyer);
// 			renderFaceUp(block, Vec3::ZERO, blockGraphics.getTexture(Facing::UP, dataVariant));
// 			renderFaceDown(block, Vec3::ZERO, blockGraphics.getTexture(Facing::DOWN, dataVariant));
// 
// 			mTessellator.color(c10 * lightMultiplyer, c10 * lightMultiplyer, c10 * lightMultiplyer, alphaMultiplyer);
// 			renderNorth(block, Vec3::ZERO, blockGraphics.getTexture(Facing::NORTH, dataVariant));
// 			renderSouth(block, Vec3::ZERO, blockGraphics.getTexture(Facing::SOUTH, dataVariant));
// 
// 			mTessellator.color(c2 * lightMultiplyer, c2 * lightMultiplyer, c2 * lightMultiplyer, alphaMultiplyer);
// 			renderWest(block, Vec3::ZERO, blockGraphics.getTexture(Facing::WEST, dataVariant));
// 			renderEast(block, Vec3::ZERO, blockGraphics.getTexture(Facing::EAST, dataVariant));
// 		}
// 
// 		t.draw(mItemMat, texture);
// 		//block.updateShape(level, 0, 0, 0);
// 	} else if(blockShape == BlockShape::PORTAL_FRAME) {
// 		int nrBoxes = EndPortalFrameBlock::hasEye(fullBlock.data) ? 2 : 1;
// 		mTessellator.begin( mce::PrimitiveMode::QuadList, 6 * 4 * nrBoxes );
// 		tessellateEndPortalFrameInWorld(static_cast<const EndPortalFrameBlock&>(block), BlockPos::ZERO, fullBlock.data);
// 		t.draw(mItemMat, texture);
// 	} else if (blockShape == BlockShape::ANVIL) {
// 		mTessellator.begin(6 * 4 * 4);
// 		tessellateAnvilInWorld(static_cast<const AnvilBlock&>(block), BlockPos::ZERO, fullBlock.data, false);
// 		t.draw(mItemMat, texture);
// 	} else if (blockShape == BlockShape::CHORUS_PLANT || blockShape == BlockShape::CHORUS_FLOWER) {
// 		mTessellator.begin(6 * 4 * 4);
// 
// 		for (int i = 0; i < 4; i++) {
// 			if (i == 0) {
// 				mCurrentShape.set(0, 0, 0, 1, 1, 1);
// 			}
// 			if (i == 1) {
// 				mCurrentShape.set(ChorusPlantBlock::PLANT_ITEM_DIMENSION, 1.0f, ChorusPlantBlock::PLANT_ITEM_DIMENSION, 1.0f- ChorusPlantBlock::PLANT_ITEM_DIMENSION, 1.0f + ChorusPlantBlock::PLANT_ITEM_DIMENSION, 1.0f - ChorusPlantBlock::PLANT_ITEM_DIMENSION);
// 			}
// 			if (i == 2) {
// 				mCurrentShape.set(-ChorusPlantBlock::PLANT_ITEM_DIMENSION, ChorusPlantBlock::PLANT_ITEM_DIMENSION, ChorusPlantBlock::PLANT_ITEM_DIMENSION, 0.0f, 1.0f - ChorusPlantBlock::PLANT_ITEM_DIMENSION, 1.0f - ChorusPlantBlock::PLANT_ITEM_DIMENSION);
// 			}
// 			if (i == 3) {
// 				mCurrentShape.set(ChorusPlantBlock::PLANT_ITEM_DIMENSION, ChorusPlantBlock::PLANT_ITEM_DIMENSION, 1.0f, 1.0f - ChorusPlantBlock::PLANT_ITEM_DIMENSION, 1.0f - ChorusPlantBlock::PLANT_ITEM_DIMENSION, 1.0f + ChorusPlantBlock::PLANT_ITEM_DIMENSION);
// 			}
// 
// 			mTessellator.color(lightMultiplyer, lightMultiplyer, lightMultiplyer, alphaMultiplyer);
// 			renderFaceDown(block, Vec3::ZERO, blockGraphics.getTexture(Facing::DOWN));
// 			renderFaceUp(block, Vec3::ZERO, blockGraphics.getTexture(Facing::UP));
// 
// 			mTessellator.color(c10 * lightMultiplyer, c10 * lightMultiplyer, c10 * lightMultiplyer, alphaMultiplyer);
// 			renderNorth(block, Vec3::ZERO, blockGraphics.getTexture(Facing::NORTH));
// 			renderSouth(block, Vec3::ZERO, blockGraphics.getTexture(Facing::SOUTH));
// 
// 			mTessellator.color(c2 * lightMultiplyer, c2 * lightMultiplyer, c2 * lightMultiplyer, alphaMultiplyer);
// 			renderWest(block, Vec3::ZERO, blockGraphics.getTexture(Facing::WEST));
// 			renderEast(block, Vec3::ZERO, blockGraphics.getTexture(Facing::EAST));
// 		}
// 
// 		t.draw(mItemMat, texture);
// 	} else if (blockShape == BlockShape::DRAGON_EGG) {
// 		mTessellator.begin(mce::PrimitiveMode::QuadList, 6 * 4 * 8);
// 		tessellateDragonEgg(block, BlockPos::ZERO, fullBlock.data, false);
// 		t.draw(mItemMat, texture);
// 	} else if (blockShape == BlockShape::END_ROD) { 
// 		mTessellator.begin(mce::PrimitiveMode::QuadList, 6 * 4 * 2);
// 		tessellateEndRodGui(block, blockGraphics);
// 		t.draw(mItemMat, texture);
// 	} else if (blockShape == BlockShape::STRUCTURE_VOID) {
// 		mTessellator.scale3d(2, 2, 2);
// 		mTessellator.addOffset(-6.0f, 4.0f, 0.0f);
// 		tessellateStructureVoidInWorld(block, BlockPos::ZERO, fullBlock.data);
// 		t.draw(mItemMat, texture);
// 		mTessellator.resetScale();
// 		mTessellator.addOffset(6.0f, -4.0f, 0.0f);
// 	}
// 
// 	mUseNormals = _mUseNormals;
// 	mRenderingGUI = false;
// 	mSmoothLighting = originalSmoothLight;
// }

bool BlockTessellator::tessellateThinFenceInWorld(const ThinFenceBlock& block, const BlockPos& p, DataID data) {
	_tex1(toUV(getLightColor(p)));
	Color col = Color::fromARGB(block.getColor(*mRegion, p));
	mTessellator.color(col);

	TextureUVCoordinateSet tex;
	TextureUVCoordinateSet edgeTex;

	const BlockGraphics& blockGraphics = *BlockGraphics::mBlocks[block.getId()];
	if (mUseFixedTexture) {
		tex = mFixedTexture;
		edgeTex = mFixedTexture;
	} else {
		tex = blockGraphics.getTexture(Facing::DOWN, block.getVariant(data));
		static const FacingID EDGE_TEXTURE = Facing::EAST;
		edgeTex = blockGraphics.getTexture(EDGE_TEXTURE, 0);
	}

	float u0 = tex._u0;	//(xt) / 256.0f;
	const float u1 = tex.offsetWidth(0.49f);//(xt + 7.99f) / 256.0f;
	const float u2 = tex._u1;//(xt + 15.99f) / 256.0f;
	const float v0 = tex._v0;//(yt) / 256.0f;
	const float v2 = tex._v1;//(yt + 15.99f) / 256.0f;

	const float iu0 = edgeTex.offsetWidth(7.0f / 16.0f);//(xet + 7) / 256.0f;
	const float iu1 = edgeTex.offsetWidth(8.99f / 16.0f);//(xet + 8.99f) / 256.0f;
	const float iv0 = edgeTex._v0;	//(yet) / 256.0f;
	const float iv1 = edgeTex.offsetHeight(0.5f);//(yet + 8) / 256.0f;
	const float iv2 = edgeTex._v1;	//(yet + 15.99f) / 256.0f;

	const float x0 = (float)p.x;
	const float x1 = x0 + .5f;
	const float x2 = x0 + 1;
	const float y0 = (float)p.y + 0.001f;
	const float y1 = y0 + 1 - 0.002f;
	const float z0 = (float)p.z;
	const float z1 = z0 + .5f;
	const float z2 = z0 + 1;
	const float ix0 = x0 + .5f - 1.0f / 16.0f;
	const float ix1 = x0 + .5f + 1.0f / 16.0f;
	const float iz0 = z0 + .5f - 1.0f / 16.0f;
	const float iz1 = z0 + .5f + 1.0f / 16.0f;

	const bool n = block.attachsTo(mRegion->getBlockID(p.north()));
	const bool s = block.attachsTo(mRegion->getBlockID(p.south()));
	const bool w = block.attachsTo(mRegion->getBlockID(p.west()));
	const bool e = block.attachsTo(mRegion->getBlockID(p.east()));

	BlockOccluder occluder(mBlockCache, block, p, mCurrentShape, { Facing::UP, Facing::DOWN }, mRenderingGUI);

	const float noZFightingOffset = 0.01f;
	auto maxHeight = mRegion->getMaxHeight();

	if ((w && e) || (!w && !e && !n && !s)) {
		mTessellator.vertexUV(x0, y1, z1, u0, v0);
		mTessellator.vertexUV(x0, y0, z1, u0, v2);
		mTessellator.vertexUV(x2, y0, z1, u2, v2);
		mTessellator.vertexUV(x2, y1, z1, u2, v0);

		if (!occluder.occludes(Facing::UP)) {
			// small edge texture
			mTessellator.vertexUV(x0, y1 + noZFightingOffset, iz1, iu1, iv2);
			mTessellator.vertexUV(x2, y1 + noZFightingOffset, iz1, iu1, iv0);
			mTessellator.vertexUV(x2, y1 + noZFightingOffset, iz0, iu0, iv0);
			mTessellator.vertexUV(x0, y1 + noZFightingOffset, iz0, iu0, iv2);
		} else {
			if (p.y < (maxHeight - 1) && mRegion->isEmptyBlock(p.x - 1, p.y + 1, p.z)) {
				mTessellator.vertexUV(x0, y1 + noZFightingOffset, iz1, iu1, iv1);
				mTessellator.vertexUV(x1, y1 + noZFightingOffset, iz1, iu1, iv2);
				mTessellator.vertexUV(x1, y1 + noZFightingOffset, iz0, iu0, iv2);
				mTessellator.vertexUV(x0, y1 + noZFightingOffset, iz0, iu0, iv1);
			}

			if (p.y < (maxHeight - 1) && mRegion->isEmptyBlock(p.x + 1, p.y + 1, p.z)) {
				mTessellator.vertexUV(x1, y1 + noZFightingOffset, iz1, iu1, iv0);
				mTessellator.vertexUV(x2, y1 + noZFightingOffset, iz1, iu1, iv1);
				mTessellator.vertexUV(x2, y1 + noZFightingOffset, iz0, iu0, iv1);
				mTessellator.vertexUV(x1, y1 + noZFightingOffset, iz0, iu0, iv0);
			}
		}

		if(!occluder.occludes(Facing::DOWN)) {
			// small edge texture
			mTessellator.vertexUV(x0, y0 - noZFightingOffset, iz1, iu1, iv2);
			mTessellator.vertexUV(x2, y0 - noZFightingOffset, iz1, iu1, iv0);
			mTessellator.vertexUV(x2, y0 - noZFightingOffset, iz0, iu0, iv0);
			mTessellator.vertexUV(x0, y0 - noZFightingOffset, iz0, iu0, iv2);
		} else {
			if (p.y > 1 && mRegion->isEmptyBlock(p.x - 1, p.y - 1, p.z)) {
				mTessellator.vertexUV(x0, y0 - noZFightingOffset, iz1, iu1, iv1);
				mTessellator.vertexUV(x1, y0 - noZFightingOffset, iz1, iu1, iv2);
				mTessellator.vertexUV(x1, y0 - noZFightingOffset, iz0, iu0, iv2);
				mTessellator.vertexUV(x0, y0 - noZFightingOffset, iz0, iu0, iv1);
			}

			if (p.y > 1 && mRegion->isEmptyBlock(p.x + 1, p.y - 1, p.z)) {
				mTessellator.vertexUV(x1, y0 - noZFightingOffset, iz1, iu1, iv0);
				mTessellator.vertexUV(x2, y0 - noZFightingOffset, iz1, iu1, iv1);
				mTessellator.vertexUV(x2, y0 - noZFightingOffset, iz0, iu0, iv1);
				mTessellator.vertexUV(x1, y0 - noZFightingOffset, iz0, iu0, iv0);
			}
		}

	} else if (w && !e) {
		// half-step towards west
		mTessellator.vertexUV(x0, y1, z1, u0, v0);
		mTessellator.vertexUV(x0, y0, z1, u0, v2);
		mTessellator.vertexUV(x1, y0, z1, u1, v2);
		mTessellator.vertexUV(x1, y1, z1, u1, v0);

		// small edge texture
		if (!s && !n) {
			mTessellator.vertexUV(x1, y1, iz1, iu0, iv0);
			mTessellator.vertexUV(x1, y0, iz1, iu0, iv2);
			mTessellator.vertexUV(x1, y0, iz0, iu1, iv2);
			mTessellator.vertexUV(x1, y1, iz0, iu1, iv0);
		}

		if(!occluder.occludes(Facing::UP) || (p.y < (maxHeight - 1) && mRegion->isEmptyBlock(p.x - 1, p.y + 1, p.z))) {
			// small edge texture
			mTessellator.vertexUV(x0, y1 + noZFightingOffset, iz1, iu1, iv1);
			mTessellator.vertexUV(x1, y1 + noZFightingOffset, iz1, iu1, iv2);
			mTessellator.vertexUV(x1, y1 + noZFightingOffset, iz0, iu0, iv2);
			mTessellator.vertexUV(x0, y1 + noZFightingOffset, iz0, iu0, iv1);
		}

		if(!occluder.occludes(Facing::DOWN) || (p.y > 1 && mRegion->isEmptyBlock(p.x - 1, p.y - 1, p.z))) {
			// small edge texture
			mTessellator.vertexUV(x0, y0 - noZFightingOffset, iz1, iu1, iv1);
			mTessellator.vertexUV(x1, y0 - noZFightingOffset, iz1, iu1, iv2);
			mTessellator.vertexUV(x1, y0 - noZFightingOffset, iz0, iu0, iv2);
			mTessellator.vertexUV(x0, y0 - noZFightingOffset, iz0, iu0, iv1);
		}

	} else if (!w && e) {
		// half-step towards east
		mTessellator.vertexUV(x1, y1, z1, u1, v0);
		mTessellator.vertexUV(x1, y0, z1, u1, v2);
		mTessellator.vertexUV(x2, y0, z1, u2, v2);
		mTessellator.vertexUV(x2, y1, z1, u2, v0);

		// small edge texture
		if (!s && !n) {
			mTessellator.vertexUV(x1, y1, iz0, iu0, iv0);
			mTessellator.vertexUV(x1, y0, iz0, iu0, iv2);
			mTessellator.vertexUV(x1, y0, iz1, iu1, iv2);
			mTessellator.vertexUV(x1, y1, iz1, iu1, iv0);
		}

		if(!occluder.occludes(Facing::UP) || (p.y < (maxHeight - 1) && mRegion->isEmptyBlock(p.x + 1, p.y + 1, p.z))) {
			// small edge texture
			mTessellator.vertexUV(x1, y1 + noZFightingOffset, iz1, iu1, iv0);
			mTessellator.vertexUV(x2, y1 + noZFightingOffset, iz1, iu1, iv1);
			mTessellator.vertexUV(x2, y1 + noZFightingOffset, iz0, iu0, iv1);
			mTessellator.vertexUV(x1, y1 + noZFightingOffset, iz0, iu0, iv0);
		}

		if(!occluder.occludes(Facing::DOWN) || (p.y > 1 && mRegion->isEmptyBlock(p.x + 1, p.y - 1, p.z))) {
			// small edge texture
			mTessellator.vertexUV(x1, y0 - noZFightingOffset, iz1, iu1, iv0);
			mTessellator.vertexUV(x2, y0 - noZFightingOffset, iz1, iu1, iv1);
			mTessellator.vertexUV(x2, y0 - noZFightingOffset, iz0, iu0, iv1);
			mTessellator.vertexUV(x1, y0 - noZFightingOffset, iz0, iu0, iv0);
		}
	}

	if ((n && s) || (!w && !e && !n && !s)) {
		// straight north-south
		mTessellator.vertexUV(x1, y1, z2, u0, v0);
		mTessellator.vertexUV(x1, y0, z2, u0, v2);
		mTessellator.vertexUV(x1, y0, z0, u2, v2);
		mTessellator.vertexUV(x1, y1, z0, u2, v0);

		if(!occluder.occludes(Facing::UP)) {
			// small edge texture
			mTessellator.vertexUV(ix1, y1, z2, iu1, iv2);
			mTessellator.vertexUV(ix1, y1, z0, iu1, iv0);
			mTessellator.vertexUV(ix0, y1, z0, iu0, iv0);
			mTessellator.vertexUV(ix0, y1, z2, iu0, iv2);
		} else {
			if (p.y < (maxHeight - 1) && mRegion->isEmptyBlock(p.x, p.y + 1, p.z - 1)) {
				mTessellator.vertexUV(ix0, y1, z0, iu1, iv0);
				mTessellator.vertexUV(ix0, y1, z1, iu1, iv1);
				mTessellator.vertexUV(ix1, y1, z1, iu0, iv1);
				mTessellator.vertexUV(ix1, y1, z0, iu0, iv0);
			}

			if (p.y < (maxHeight - 1) && mRegion->isEmptyBlock(p.x, p.y + 1, p.z + 1)) {
				mTessellator.vertexUV(ix0, y1, z1, iu0, iv1);
				mTessellator.vertexUV(ix0, y1, z2, iu0, iv2);
				mTessellator.vertexUV(ix1, y1, z2, iu1, iv2);
				mTessellator.vertexUV(ix1, y1, z1, iu1, iv1);
			}
		}

		if(!occluder.occludes(Facing::DOWN)) {
			// small edge texture
			mTessellator.vertexUV(ix1, y0, z2, iu1, iv2);
			mTessellator.vertexUV(ix1, y0, z0, iu1, iv0);
			mTessellator.vertexUV(ix0, y0, z0, iu0, iv0);
			mTessellator.vertexUV(ix0, y0, z2, iu0, iv2);
		} else {
			if (p.y > 1 && mRegion->isEmptyBlock(p.x, p.y - 1, p.z - 1)) {
				// north half-step
				mTessellator.vertexUV(ix0, y0, z0, iu1, iv0);
				mTessellator.vertexUV(ix0, y0, z1, iu1, iv1);
				mTessellator.vertexUV(ix1, y0, z1, iu0, iv1);
				mTessellator.vertexUV(ix1, y0, z0, iu0, iv0);
			}

			if (p.y > 1 && mRegion->isEmptyBlock(p.x, p.y - 1, p.z + 1)) {
				// south half-step
				mTessellator.vertexUV(ix0, y0, z1, iu0, iv1);
				mTessellator.vertexUV(ix0, y0, z2, iu0, iv2);
				mTessellator.vertexUV(ix1, y0, z2, iu1, iv2);
				mTessellator.vertexUV(ix1, y0, z1, iu1, iv1);
			}
		}
	} else if (n && !s) {
		// half-step towards north
		mTessellator.vertexUV(x1, y1, z0, u0, v0);
		mTessellator.vertexUV(x1, y0, z0, u0, v2);
		mTessellator.vertexUV(x1, y0, z1, u1, v2);
		mTessellator.vertexUV(x1, y1, z1, u1, v0);

		// small edge texture
		if (!e && !w) {
			mTessellator.vertexUV(ix0, y1, z1, iu0, iv0);
			mTessellator.vertexUV(ix0, y0, z1, iu0, iv2);
			mTessellator.vertexUV(ix1, y0, z1, iu1, iv2);
			mTessellator.vertexUV(ix1, y1, z1, iu1, iv0);
		}

		if(!occluder.occludes(Facing::UP) || (p.y < (maxHeight - 1) && mRegion->isEmptyBlock(p.x, p.y + 1, p.z - 1))) {
			// small edge texture
			mTessellator.vertexUV(ix0, y1, z0, iu1, iv0);
			mTessellator.vertexUV(ix0, y1, z1, iu1, iv1);
			mTessellator.vertexUV(ix1, y1, z1, iu0, iv1);
			mTessellator.vertexUV(ix1, y1, z0, iu0, iv0);
		}

		if(!occluder.occludes(Facing::DOWN) || (p.y > 1 && mRegion->isEmptyBlock(p.x, p.y - 1, p.z - 1))) {
			// small edge texture
			mTessellator.vertexUV(ix0, y0, z0, iu1, iv0);
			mTessellator.vertexUV(ix0, y0, z1, iu1, iv1);
			mTessellator.vertexUV(ix1, y0, z1, iu0, iv1);
			mTessellator.vertexUV(ix1, y0, z0, iu0, iv0);
		}

	} else if (!n && s) {
		// half-step towards south
		mTessellator.vertexUV(x1, y1, z1, u1, v0);
		mTessellator.vertexUV(x1, y0, z1, u1, v2);
		mTessellator.vertexUV(x1, y0, z2, u2, v2);
		mTessellator.vertexUV(x1, y1, z2, u2, v0);

		// small edge texture
		if (!e && !w) {
			mTessellator.vertexUV(ix1, y1, z1, iu0, iv0);
			mTessellator.vertexUV(ix1, y0, z1, iu0, iv2);
			mTessellator.vertexUV(ix0, y0, z1, iu1, iv2);
			mTessellator.vertexUV(ix0, y1, z1, iu1, iv0);
		}

		if(!occluder.occludes(Facing::UP) || (p.y < (maxHeight - 1) && mRegion->isEmptyBlock(p.x, p.y + 1, p.z + 1))) {
			// small edge texture
			mTessellator.vertexUV(ix0, y1, z1, iu0, iv1);
			mTessellator.vertexUV(ix0, y1, z2, iu0, iv2);
			mTessellator.vertexUV(ix1, y1, z2, iu1, iv2);
			mTessellator.vertexUV(ix1, y1, z1, iu1, iv1);
		}

		if(!occluder.occludes(Facing::DOWN) || (p.y > 1 && mRegion->isEmptyBlock(p.x, p.y - 1, p.z + 1))) {
			// small edge texture
			mTessellator.vertexUV(ix0, y0, z1, iu0, iv1);
			mTessellator.vertexUV(ix0, y0, z2, iu0, iv2);
			mTessellator.vertexUV(ix1, y0, z2, iu1, iv2);
			mTessellator.vertexUV(ix1, y0, z1, iu1, iv1);
		}
	}

	return true;
}

void BlockTessellator::tessellateRowTexture(const Block& block, DataID data, float x, float y, float z) {
	const BlockGraphics& blockGraphics = *BlockGraphics::mBlocks[block.getId()];
	const TextureUVCoordinateSet& tex = mUseFixedTexture ? mFixedTexture : blockGraphics.getTexture(Facing::DOWN, block.getVariant(data));

	float u0 = tex._u0;	//(xt) / 256.0f;
	float u1 = tex._u1;	//(xt + 15.99f) / 256.f;
	float v0 = tex._v0;	//(yt) / 256.0f;
	float v1 = tex._v1;	//(yt + 15.99f) / 256.0f;

	float x0 = x + 0.5f - 0.25f;
	float x1 = x + 0.5f + 0.25f;
	float z0 = z + 0.5f - 0.5f;
	float z1 = z + 0.5f + 0.5f;
	mTessellator.vertexUV(x0, y + 1, z0, u0, v0);
	mTessellator.vertexUV(x0, y + 0, z0, u0, v1);
	mTessellator.vertexUV(x0, y + 0, z1, u1, v1);
	mTessellator.vertexUV(x0, y + 1, z1, u1, v0);

	mTessellator.vertexUV(x1, y + 1, z1, u0, v0);
	mTessellator.vertexUV(x1, y + 0, z1, u0, v1);
	mTessellator.vertexUV(x1, y + 0, z0, u1, v1);
	mTessellator.vertexUV(x1, y + 1, z0, u1, v0);

	x0 = x + 0.5f - 0.5f;
	x1 = x + 0.5f + 0.5f;
	z0 = z + 0.5f - 0.25f;
	z1 = z + 0.5f + 0.25f;

	mTessellator.vertexUV(x0, y + 1, z0, u0, v0);
	mTessellator.vertexUV(x0, y + 0, z0, u0, v1);
	mTessellator.vertexUV(x1, y + 0, z0, u1, v1);
	mTessellator.vertexUV(x1, y + 1, z0, u1, v0);

	mTessellator.vertexUV(x1, y + 1, z1, u0, v0);
	mTessellator.vertexUV(x1, y + 0, z1, u0, v1);
	mTessellator.vertexUV(x0, y + 0, z1, u1, v1);
	mTessellator.vertexUV(x0, y + 1, z1, u1, v0);
}

bool BlockTessellator::tessellateTreeInWorld(const Block& block, const BlockPos& p, DataID data) {
	int facing = Block::mQuartzBlock->getBlockState(BlockState::Direction).get<int>(data);

	if (facing == RotatedPillarBlock::FACING_X) {
		mFlipFace[Facing::NORTH] = FLIP_CW;
		mFlipFace[Facing::SOUTH] = FLIP_CW;
		mFlipFace[Facing::UP] = FLIP_CW;
		mFlipFace[Facing::DOWN] = FLIP_CW;
	} else if (facing == RotatedPillarBlock::FACING_Z) {
		mFlipFace[Facing::EAST] = FLIP_CW;
		mFlipFace[Facing::WEST] = FLIP_CW;
	}

	return tessellateBlockInWorld(block, p, data);
}

bool BlockTessellator::tessellateWallInWorld(const WallBlock& block, const BlockPos& p, int data) {
	bool w = block.connectsTo(*mRegion, p, p.west());
	bool e = block.connectsTo(*mRegion, p, p.east());
	bool n = block.connectsTo(*mRegion, p, p.north());
	bool s = block.connectsTo(*mRegion, p, p.south());

	bool vertical = (n && s && !w && !e);
	bool horizontal = (!n && !s && w && e);
	bool emptyAbove = mRegion->isEmptyBlock(p.above());

	if((!vertical && !horizontal) || !emptyAbove) {
		// center post
		{
			BlockOccluder tc(mBlockCache, block, p, mCurrentShape, { Facing::UP, Facing::DOWN }, mRenderingGUI);
			mCurrentShape.set(.5f - WallBlock::POST_WIDTH, 0, .5f - WallBlock::POST_WIDTH, .5f + WallBlock::POST_WIDTH, WallBlock::POST_HEIGHT, .5f + WallBlock::POST_WIDTH);
			tessellateBlockInWorld(block, p, data);
		}

		{
			BlockOccluder tc(mBlockCache, block, p, mCurrentShape, {}, mRenderingGUI);
			if(w) {
				mCurrentShape.set(0, 0, .5f - WallBlock::WALL_WIDTH, .5f - WallBlock::POST_WIDTH, WallBlock::WALL_HEIGHT, .5f + WallBlock::WALL_WIDTH);
				tessellateBlockInWorld(block, p, data);
			}

			if(e) {
				mCurrentShape.set(.5f + WallBlock::POST_WIDTH, 0, .5f - WallBlock::WALL_WIDTH, 1, WallBlock::WALL_HEIGHT, .5f + WallBlock::WALL_WIDTH);
				tessellateBlockInWorld(block, p, data);
			}

			if(n) {
				mCurrentShape.set(.5f - WallBlock::WALL_WIDTH, 0, 0, .5f + WallBlock::WALL_WIDTH, WallBlock::WALL_HEIGHT, .5f - WallBlock::POST_WIDTH);
				tessellateBlockInWorld(block, p, data);
			}

			if(s) {
				mCurrentShape.set(.5f - WallBlock::WALL_WIDTH, 0, .5f + WallBlock::POST_WIDTH, .5f + WallBlock::WALL_WIDTH, WallBlock::WALL_HEIGHT, 1);
				tessellateBlockInWorld(block, p, data);
			}
		}
	} else if(vertical) {
		// north-south wall
		mCurrentShape.set(.5f - WallBlock::WALL_WIDTH, 0, 0, .5f + WallBlock::WALL_WIDTH, WallBlock::WALL_HEIGHT, 1);
		tessellateBlockInWorld(block, p, data);
	} else {
		// west-east wall
		mCurrentShape.set(0, 0, .5f - WallBlock::WALL_WIDTH, 1, WallBlock::WALL_HEIGHT, .5f + WallBlock::WALL_WIDTH);
		tessellateBlockInWorld(block, p, data);
	}

	return true;
}

bool BlockTessellator::tessellateRailInWorld(const BaseRailBlock& block, const BlockPos& p, int data) {
	Vec3 v = p;

	const BlockGraphics& blockGraphics = *BlockGraphics::mBlocks[block.getId()];
	TextureUVCoordinateSet tex = blockGraphics.getTexture(block.getVariant(data), 0);

	if (mUseFixedTexture) {
		tex = mFixedTexture;
	}

	if (block.isUsesDataBit()) {
		data = Block::mRail->getBlockState(BlockState::RailDirection).get<int>(data);
	}

	_tex1(toUV(getLightColor(p)));
	mTessellator.color(Color::SHADE_UP);

	float u0 = tex._u0;
	float v0 = tex._v0;
	float u1 = tex._u1;
	float v1 = tex._v1;

	float r = 1 / 16.0;

	float x0 = v.x + 1.0f;
	float x1 = v.x + 1.0f;
	float x2 = v.x + 0.0f;
	float x3 = v.x + 0.0f;

	float z0 = v.z + 0.0f;
	float z1 = v.z + 1.0f;
	float z2 = v.z + 1.0f;
	float z3 = v.z + 0.0f;

	float y0 = v.y + r;
	float y1 = v.y + r;
	float y2 = v.y + r;
	float y3 = v.y + r;

	if (data == 1 || data == 2 || data == 3 || data == 7) {
		x0 = x3 = v.x + 1.0f;
		x1 = x2 = v.x + 0.0f;
		z0 = z1 = v.z + 1.0f;
		z2 = z3 = v.z + 0.0f;
	} else if (data == 8) {
		x0 = x1 = v.x + 0.0f;
		x2 = x3 = v.x + 1.0f;
		z0 = z3 = v.z + 1.0f;
		z1 = z2 = v.z + 0.0f;
	} else if (data == 9) {
		x0 = x3 = v.x + 0.0f;
		x1 = x2 = v.x + 1.0f;
		z0 = z1 = v.z + 0.0f;
		z2 = z3 = v.z + 1.0f;
	}

	if (data == 2 || data == 4) {
		y0 += 1.0f;
		y3 += 1.0f;
	} else if (data == 3 || data == 5) {
		y1 += 1.0f;
		y2 += 1.0f;
	}

	mTessellator.vertexUV(x0, y0, z0, u1, v0);
	mTessellator.vertexUV(x1, y1, z1, u1, v1);
	mTessellator.vertexUV(x2, y2, z2, u0, v1);
	mTessellator.vertexUV(x3, y3, z3, u0, v0);

	mTessellator.vertexUV(x3, y3, z3, u0, v0);
	mTessellator.vertexUV(x2, y2, z2, u0, v1);
	mTessellator.vertexUV(x1, y1, z1, u1, v1);
	mTessellator.vertexUV(x0, y0, z0, u1, v0);

	return true;
}

void BlockTessellator::clearBlockCache() {
// 	mBlockMeshes.clear();
}

void BlockTessellator::resetCache(BlockPos const& pos, BlockSource& region) {
	mBlockCache.reset(region, pos);
}

Tessellator& BlockTessellator::getTessellator() {
	return mTessellator;
}
const TextureUVCoordinateSet& BlockTessellator::_getTexture(const Block& block, FacingID face, int data) const {
	const BlockGraphics& blockGraphics = *BlockGraphics::mBlocks[block.getId()];
    FacingID mappedFacingID = block.getMappedFace(face, data);
	return mRenderingGUI ? blockGraphics.getTexture(face, block.getVariant(mRenderingData))
	                     : blockGraphics.getTexture(mappedFacingID, block.getVariant(data));
}
DataID BlockTessellator::getData(const BlockPos& p) const {
	return mRenderingGUI ? mRenderingData : mRegion->getData(p);
}

bool BlockTessellator::tessellateLeverInWorld(const Block& block, const BlockPos& pos, int data) {
	int dir = data & 7;
	bool flipped = (data & 8) > 0;

	bool hadFixedTexture = mUseFixedTexture;
	if (!mUseFixedTexture) {
		mUseFixedTexture = true;
		mFixedTexture = _getTexture(*Block::mCobblestoneWall, Facing::UP, 0);
	}

	float w1 = 4 / 16.0f;
	float w2 = 3 / 16.0f;
	float h = 3 / 16.0f;
	if (dir == 5) {
		mCurrentShape.set(0.5f - w2, 0, 0.5f - w1, 0.5f + w2, h, 0.5f + w1);
	} else if (dir == 6) {
		mCurrentShape.set(0.5f - w1, 0, 0.5f - w2, 0.5f + w1, h, 0.5f + w2);
	} else if (dir == 4) {
		mCurrentShape.set(0.5f - w2, 0.5f - w1, 1 - h, 0.5f + w2, 0.5f + w1, 1);
	} else if (dir == 3) {
		mCurrentShape.set(0.5f - w2, 0.5f - w1, 0, 0.5f + w2, 0.5f + w1, h);
	} else if (dir == 2) {
		mCurrentShape.set(1 - h, 0.5f - w1, 0.5f - w2, 1, 0.5f + w1, 0.5f + w2);
	} else if (dir == 1) {
		mCurrentShape.set(0, 0.5f - w1, 0.5f - w2, h, 0.5f + w1, 0.5f + w2);
	} else if (dir == 0) {
		mCurrentShape.set(0.5f - w1, 1 - h, 0.5f - w2, 0.5f + w1, 1, 0.5f + w2);
	} else if (dir == 7) {
		mCurrentShape.set(0.5f - w2, 1 - h, 0.5f - w1, 0.5f + w2, 1, 0.5f + w1);
	}
	tessellateBlockInWorld(block, pos, data);
	
	if (!hadFixedTexture) {
		mUseFixedTexture = false;
	}

	_tex1(toUV(getLightColor(pos)));
	mTessellator.color(1.0f, 1.0f, 1.0f);
	const TextureUVCoordinateSet &tex = mUseFixedTexture ? mFixedTexture : BlockGraphics::mBlocks[block.getId()]->getTexture(0);

	// Not all blocks are 16x16 anymore
	float scale = tex.pixelWidth() / 16.f;

	float u0 = tex._u0;
	float v0 = tex._v0;
	float u1 = tex._u1;
	float v1 = tex._v1;

	Vec3 corners[8];
	float xv = 1 / 16.0f;
	float zv = 1 / 16.0f;
	float yv = 10 / 16.0f;
	corners[0].set(-xv, -0, -zv);
	corners[1].set(+xv, -0, -zv);
	corners[2].set(+xv, -0, +zv);
	corners[3].set(-xv, -0, +zv);
	corners[4].set(-xv, +yv, -zv);
	corners[5].set(+xv, +yv, -zv);
	corners[6].set(+xv, +yv, +zv);
	corners[7].set(-xv, +yv, +zv);

	for (int i = 0; i < 8; i++) {
		if (flipped) {
			corners[i].z -= 1 / 16.0f;
			corners[i].xRot(40 * PI / 180);
		} else {
			corners[i].z += 1 / 16.0f;
			corners[i].xRot(-40 * PI / 180);
		}
		if (dir == 0 || dir == 7) {
			corners[i].zRot(180 * PI / 180);
		}
		if (dir == 6 || dir == 0) {
			corners[i].yRot(90 * PI / 180);
		}

		if (dir > 0 && dir < 5) {
			corners[i].y -= 6 / 16.0f;
			corners[i].xRot(90 * PI / 180);

			if (dir == 4) {
				corners[i].yRot(0 * PI / 180);
			}
			if (dir == 3) {
				corners[i].yRot(180 * PI / 180);
			}
			if (dir == 2) {
				corners[i].yRot(90 * PI / 180);
			}
			if (dir == 1) {
				corners[i].yRot(-90 * PI / 180);
			}

			corners[i].x += pos.x + 0.5f;
			corners[i].y += pos.y + 8.0f / 16.0f;
			corners[i].z += pos.z + 0.5f;
		} else if (dir == 0 || dir == 7) {
			corners[i].x += pos.x + 0.5f;
			corners[i].y += pos.y + 14.0f / 16.0f;
			corners[i].z += pos.z + 0.5f;
		} else {
			corners[i].x += pos.x + 0.5f;
			corners[i].y += pos.y + 2.0f / 16.0f;
			corners[i].z += pos.z + 0.5f;
		}
	}

	Vec3 c0, c1, c2, c3;
	for (int i = 0; i < 6; i++) {
		if (i == 0) {
			u0 = tex.offsetWidthPixel(7 * scale);
			v0 = tex.offsetHeightPixel(6 * scale);
			u1 = tex.offsetWidthPixel(9 * scale);
			v1 = tex.offsetHeightPixel(8 * scale);
		} else if (i == 2) {
			u0 = tex.offsetWidthPixel(7 * scale);
			v0 = tex.offsetHeightPixel(6 * scale);
			u1 = tex.offsetWidthPixel(9 * scale);
			v1 = tex._v1;
		}
		if (i == 0) {
			c0 = corners[0];
			c1 = corners[1];
			c2 = corners[2];
			c3 = corners[3];
		} else if (i == 1) {
			c0 = corners[7];
			c1 = corners[6];
			c2 = corners[5];
			c3 = corners[4];
		} else if (i == 2) {
			c0 = corners[1];
			c1 = corners[0];
			c2 = corners[4];
			c3 = corners[5];
		} else if (i == 3) {
			c0 = corners[2];
			c1 = corners[1];
			c2 = corners[5];
			c3 = corners[6];
		} else if (i == 4) {
			c0 = corners[3];
			c1 = corners[2];
			c2 = corners[6];
			c3 = corners[7];
		} else if (i == 5) {
			c0 = corners[0];
			c1 = corners[3];
			c2 = corners[7];
			c3 = corners[4];
		}
		mTessellator.vertexUV(c0.x, c0.y, c0.z, u0 + TEXTURE_OFFSET, v1 - TEXTURE_OFFSET);
		mTessellator.vertexUV(c1.x, c1.y, c1.z, u1 - TEXTURE_OFFSET, v1 - TEXTURE_OFFSET);
		mTessellator.vertexUV(c2.x, c2.y, c2.z, u1 - TEXTURE_OFFSET, v0);
		mTessellator.vertexUV(c3.x, c3.y, c3.z, u0 + TEXTURE_OFFSET, v0);
	}
	return true;
}

bool BlockTessellator::tessellateDustInWorld(const Block& b, const BlockPos &p, DataID data) {
	static const FacingID CROSS = Facing::UP;
	static const FacingID LINE = Facing::DOWN;
	const TextureUVCoordinateSet &crossTexture = BlockGraphics::mBlocks[b.getId()]->getTexture(CROSS, 0);
	const TextureUVCoordinateSet &lineTexture = BlockGraphics::mBlocks[b.getId()]->getTexture(LINE, 0);

	_tex1(toUV(getLightColor(p)));

	Color color = Color::fromARGB(b.getColor(data));
	mTessellator.color(color.r, color.g, color.b);

	const float dustOffset = 0.25f / 16.0f;
	const float lineCutDist = 6.0f / 16.0f;

	BlockPos west = p.west();
	BlockPos east = p.east();
	BlockPos south = p.south();
	BlockPos north = p.north();

	bool w = RedStoneWireBlock::shouldConnectTo(*mRegion, west, Direction::WEST) || (!mRegion->isSolidBlockingBlock(west) && RedStoneWireBlock::shouldConnectTo(*mRegion, west.below(), Direction::UNDEFINED) && mRegion->getBlockID(west.below()) == Block::mRedStoneDust->mID);
	bool e = RedStoneWireBlock::shouldConnectTo(*mRegion, east, Direction::EAST) || (!mRegion->isSolidBlockingBlock(east) && RedStoneWireBlock::shouldConnectTo(*mRegion, east.below(), Direction::UNDEFINED) && mRegion->getBlockID(east.below()) == Block::mRedStoneDust->mID);
	bool s = RedStoneWireBlock::shouldConnectTo(*mRegion, south, Direction::SOUTH) || (!mRegion->isSolidBlockingBlock(south) && RedStoneWireBlock::shouldConnectTo(*mRegion, south.below(), Direction::UNDEFINED) && mRegion->getBlockID(south.below()) == Block::mRedStoneDust->mID);
	bool n = RedStoneWireBlock::shouldConnectTo(*mRegion, north, Direction::NORTH) || (!mRegion->isSolidBlockingBlock(north) && RedStoneWireBlock::shouldConnectTo(*mRegion, north.below(), Direction::UNDEFINED) && mRegion->getBlockID(north.below()) == Block::mRedStoneDust->mID);

	if (!mRegion->isSolidBlockingBlock( p.above() ) ) {
		if (b.canSurvive(*mRegion, west.above()) && RedStoneWireBlock::shouldConnectTo(*mRegion, west.above(), Direction::UNDEFINED) && mRegion->getBlockID(west.above()) == Block::mRedStoneDust->mID) {
			w = true;
		}
		if (b.canSurvive(*mRegion, east.above()) && RedStoneWireBlock::shouldConnectTo(*mRegion, east.above(), Direction::UNDEFINED) && mRegion->getBlockID(east.above()) == Block::mRedStoneDust->mID) {
			e = true;
		}
		if (b.canSurvive(*mRegion, north.above()) && RedStoneWireBlock::shouldConnectTo(*mRegion, north.above(), Direction::UNDEFINED) && mRegion->getBlockID(north.above()) == Block::mRedStoneDust->mID) {
			n = true;
		}
		if (b.canSurvive(*mRegion, south.above()) && RedStoneWireBlock::shouldConnectTo(*mRegion, south.above(), Direction::UNDEFINED) && mRegion->getBlockID(south.above()) == Block::mRedStoneDust->mID) {
			s = true;
		}
	}

	float x0 = (float)(p.x + 0);
	float x1 = (float)(p.x + 1);
	float z0 = (float)(p.z + 0);
	float z1 = (float)(p.z + 1);
	float y = (float)p.y;

	// Trimmed bounds for straight redstone wire
	float lineCutX0 = x0 + lineCutDist;
	float lineCutX1 = x1 - lineCutDist;
	float lineCutZ0 = z0 + lineCutDist;
	float lineCutZ1 = z1 - lineCutDist;

	float lineCutV0 = lineTexture.offsetHeight(lineCutDist);
	float lineCutV1 = lineTexture.inverseOffsetHeight(lineCutDist);

	int pic = 0;
	if ((w || e) && (!n && !s)) {
		pic = 1;
	}
	if ((n || s) && (!e && !w)) {
		pic = 2;
	}

	if (pic == 0) {
		float u0 = crossTexture._u0;
		float v0 = crossTexture._v0;
		float u1 = crossTexture._u1;
		float v1 = crossTexture._v1;

		// Not all blocks are 16x16 anymore
		int scale = crossTexture.pixelWidth() / 16;
		const float cutDistance = (5.0f * scale) / (16.0f * scale);
		TextureUVCoordinateSet offsetCrossTexture = crossTexture.subTexture(5.f * scale, 5.f * scale, (16 * scale) - (10 * scale), (16 * scale) - (10 * scale));

		//	Make sure that if there are no blocks we should connect to around us we display the redstone dust dot with arms off in each direction.
		//	A blast form the past! :D
		if (w || e || n || s) {
			if (!w) {
				x0 += cutDistance;
				u0 = offsetCrossTexture._u0;
			}
			if (!e) {
				x1 -= cutDistance;
				u1 = offsetCrossTexture._u1;
			}
			if (!n) {
				z0 += cutDistance;
				v0 = offsetCrossTexture._v0;
			}
			if (!s) {
				z1 -= cutDistance;
				v1 = offsetCrossTexture._v1;
			}
		}

		mTessellator.vertexUV(x1, y + dustOffset, z1, u1, v1);
		mTessellator.vertexUV(x1, y + dustOffset, z0, u1, v0);
		mTessellator.vertexUV(x0, y + dustOffset, z0, u0, v0);
		mTessellator.vertexUV(x0, y + dustOffset, z1, u0, v1);
	}
	else if (pic == 1) {
		mTessellator.vertexUV(x1, y + dustOffset, lineCutZ1, lineTexture._u1, lineCutV1);
		mTessellator.vertexUV(x1, y + dustOffset, lineCutZ0, lineTexture._u1, lineCutV0);
		mTessellator.vertexUV(x0, y + dustOffset, lineCutZ0, lineTexture._u0, lineCutV0);
		mTessellator.vertexUV(x0, y + dustOffset, lineCutZ1, lineTexture._u0, lineCutV1);
	}
	else {
		mTessellator.vertexUV(lineCutX1, y + dustOffset, z1, lineTexture._u1, lineCutV1);
		mTessellator.vertexUV(lineCutX1, y + dustOffset, z0, lineTexture._u0, lineCutV1);
		mTessellator.vertexUV(lineCutX0, y + dustOffset, z0, lineTexture._u0, lineCutV0);
		mTessellator.vertexUV(lineCutX0, y + dustOffset, z1, lineTexture._u1, lineCutV0);
	}

	if (!mRegion->isConsideredSolidBlock(p.above())) {
		const float yStretch = .35f / 16.0f;

		if ((mRegion->isConsideredSolidBlock(west)) && mRegion->getBlockID(west.above()) == Block::mRedStoneDust->mID) {
			mTessellator.color(color.r, color.g, color.b);
			mTessellator.vertexUV(x0 + dustOffset, y + 1 + yStretch, lineCutZ1, lineTexture._u1, lineCutV0);
			mTessellator.vertexUV(x0 + dustOffset, y + 0, lineCutZ1, lineTexture._u0, lineCutV0);
			mTessellator.vertexUV(x0 + dustOffset, y + 0, lineCutZ0, lineTexture._u0, lineCutV1);
			mTessellator.vertexUV(x0 + dustOffset, y + 1 + yStretch, lineCutZ0, lineTexture._u1, lineCutV1);
		}
		if (mRegion->isConsideredSolidBlock(east) && mRegion->getBlockID(east.above()) == Block::mRedStoneDust->mID) {
			mTessellator.color(color.r, color.g, color.b);
			mTessellator.vertexUV(x1 - dustOffset, y + 0, lineCutZ1, lineTexture._u0, lineCutV1);
			mTessellator.vertexUV(x1 - dustOffset, y + 1 + yStretch, lineCutZ1, lineTexture._u1, lineCutV1);
			mTessellator.vertexUV(x1 - dustOffset, y + 1 + yStretch, lineCutZ0, lineTexture._u1, lineCutV0);
			mTessellator.vertexUV(x1 - dustOffset, y + 0, lineCutZ0, lineTexture._u0, lineCutV0);
		}
		if (mRegion->isConsideredSolidBlock(north) && mRegion->getBlockID(north.above()) == Block::mRedStoneDust->mID) {
			mTessellator.color(color.r, color.g, color.b);
			mTessellator.vertexUV(lineCutX1, y + 0, z0 + dustOffset, lineTexture._u0, lineCutV1);
			mTessellator.vertexUV(lineCutX1, y + 1 + yStretch, z0 + dustOffset, lineTexture._u1, lineCutV1);
			mTessellator.vertexUV(lineCutX0, y + 1 + yStretch, z0 + dustOffset, lineTexture._u1, lineCutV0);
			mTessellator.vertexUV(lineCutX0, y + 0, z0 + dustOffset, lineTexture._u0, lineCutV0);
		}
		if (mRegion->isConsideredSolidBlock(south) && mRegion->getBlockID(south.above()) == Block::mRedStoneDust->mID) {
			mTessellator.color(color.r, color.g, color.b);
			mTessellator.vertexUV(lineCutX1, y + 1 + yStretch, z1 - dustOffset, lineTexture._u1, lineCutV0);
			mTessellator.vertexUV(lineCutX1, y + 0, z1 - dustOffset, lineTexture._u0, lineCutV0);
			mTessellator.vertexUV(lineCutX0, y + 0, z1 - dustOffset, lineTexture._u0, lineCutV1);
			mTessellator.vertexUV(lineCutX0, y + 1 + yStretch, z1 - dustOffset, lineTexture._u1, lineCutV1);
		}
	}

	return true;
}

bool BlockTessellator::tessellateTripwireHookInWorld(const Block& b, const BlockPos& p, int data) {
	int dir = Block::mTripwireHook->getBlockState(BlockState::Direction).get<int>(data);
	bool attached = Block::mTripwireHook->getBlockState(BlockState::AttachedBit).getBool(data);
	bool powered = Block::mTripwireHook->getBlockState(BlockState::PoweredBit).getBool(data);
	
	const BlockGraphics& blockGraphics = *BlockGraphics::mBlocks[b.getId()];
	bool hadfixed = false;
	if (!hadfixed) {
		mUseFixedTexture = true;
		const static FacingID TRIP_WIRE_BASE = Facing::DOWN;
		mFixedTexture = blockGraphics.getTexture(TRIP_WIRE_BASE);
	}

	float boxHeight = 4 / 16.0f;
	float boxWidth = 2 / 16.0f;
	float boxDepth = 2 / 16.0f;

	float boxy0 = 0.3f - boxHeight;
	float boxy1 = 0.3f + boxHeight;
	if (dir == Direction::NORTH) {
		mCurrentShape.set(0.5f - boxWidth, boxy0, 1 - boxDepth, 0.5f + boxWidth, boxy1, 1);
	}
	else if (dir == Direction::SOUTH) {
		mCurrentShape.set(0.5f - boxWidth, boxy0, 0, 0.5f + boxWidth, boxy1, boxDepth);
	}
	else if (dir == Direction::WEST) {
		mCurrentShape.set(1 - boxDepth, boxy0, 0.5f - boxWidth, 1, boxy1, 0.5f + boxWidth);
	}
	else if (dir == Direction::EAST) {
		mCurrentShape.set(0, boxy0, 0.5f - boxWidth, boxDepth, boxy1, 0.5f + boxWidth);
	}

	tessellateBlockInWorld(b, p, data);
	if (!hadfixed) {
		mUseFixedTexture = false;
		mFixedTexture = TextureUVCoordinateSet();
	}

	_tex1(toUV(getLightColor(p)));
	mTessellator.color(1.0f, 1.0f, 1.0f);
	const static FacingID TRIP_WIRE_SOURCE = Facing::UP;
	const TextureUVCoordinateSet& tex = mUseFixedTexture ? mFixedTexture : blockGraphics.getTexture(TRIP_WIRE_SOURCE);

	float u0 = tex._u0;
	float v0 = tex._v0;
	float u1 = tex._u1;
	float v1 = tex._v1;

	// Not all blocks are 16x16 anymore
	float scale = tex.pixelWidth() / 16.f;

	Vec3 corners[8];
	float stickWidth = 0.75f / 16.0f;
	float stickHeight = 0.75f / 16.0f;
	float stickLength = 5 / 16.0f;
	corners[0].set(-stickWidth, -0, -stickHeight);
	corners[1].set(+stickWidth, -0, -stickHeight);
	corners[2].set(+stickWidth, -0, +stickHeight);
	corners[3].set(-stickWidth, -0, +stickHeight);
	corners[4].set(-stickWidth, +stickLength, -stickHeight);
	corners[5].set(+stickWidth, +stickLength, -stickHeight);
	corners[6].set(+stickWidth, +stickLength, +stickHeight);
	corners[7].set(-stickWidth, +stickLength, +stickHeight);

	for (int i = 0; i < 8; i++) {
		corners[i].z += 1 / 16.0f;

		if (powered) {
			corners[i].xRot(30 * PI / 180);
			corners[i].y -= 7 / 16.0f;
		}
		else if (attached) {
			corners[i].xRot(5 * PI / 180);
			corners[i].y -= 7 / 16.0f;
		}
		else {
			corners[i].xRot(-40 * PI / 180);
			corners[i].y -= 6 / 16.0f;
		}

		corners[i].xRot(90 * PI / 180);

		if (dir == Direction::NORTH) {
			corners[i].yRot(0 * PI / 180);
		}
		if (dir == Direction::SOUTH) {
			corners[i].yRot(180 * PI / 180);
		}
		if (dir == Direction::WEST) {
			corners[i].yRot(90 * PI / 180);
		}
		if (dir == Direction::EAST) {
			corners[i].yRot(-90 * PI / 180);
		}

		corners[i].x += p.x + 0.5f;
		corners[i].y += p.y + 5.0f / 16.0f;
		corners[i].z += p.z + 0.5f;
	}

	Vec3 c0, c1, c2, c3;
	float stickX0 = 7.f * scale;
	float stickX1 = 9.f * scale;
	float stickY0 = 9.f * scale;
	float stickY1 = 16.f * scale;

	for (int i = 0; i < 6; i++) {
		if (i == 0) {
			c0 = corners[0];
			c1 = corners[1];
			c2 = corners[2];
			c3 = corners[3];
			u0 = tex.offsetWidthPixel(stickX0);
			v0 = tex.offsetHeightPixel(stickY0);
			u1 = tex.offsetWidthPixel(stickX1);
			v1 = tex.offsetHeightPixel(stickY0 + 2.f * scale);
		}
		else if (i == 1) {
			c0 = corners[7];
			c1 = corners[6];
			c2 = corners[5];
			c3 = corners[4];
		}
		else if (i == 2) {
			c0 = corners[1];
			c1 = corners[0];
			c2 = corners[4];
			c3 = corners[5];
			u0 = tex.offsetWidthPixel(stickX0);
			v0 = tex.offsetHeightPixel(stickY0);
			u1 = tex.offsetWidthPixel(stickX1);
			v1 = tex.offsetHeightPixel(stickY1);
		}
		else if (i == 3) {
			c0 = corners[2];
			c1 = corners[1];
			c2 = corners[5];
			c3 = corners[6];
		}
		else if (i == 4) {
			c0 = corners[3];
			c1 = corners[2];
			c2 = corners[6];
			c3 = corners[7];
		}
		else if (i == 5) {
			c0 = corners[0];
			c1 = corners[3];
			c2 = corners[7];
			c3 = corners[4];
		}
		mTessellator.vertexUV(c0.x, c0.y, c0.z, u0, v1);
		mTessellator.vertexUV(c1.x, c1.y, c1.z, u1, v1);
		mTessellator.vertexUV(c2.x, c2.y, c2.z, u1, v0);
		mTessellator.vertexUV(c3.x, c3.y, c3.z, u0, v0);
	}


	float hoopWidth = 1.5f / 16.0f;
	float hoopHeight = 1.5f / 16.0f;
	float hoopLength = 0.5f / 16.0f;
	corners[0].set(-hoopWidth, -0, -hoopHeight);
	corners[1].set(+hoopWidth, -0, -hoopHeight);
	corners[2].set(+hoopWidth, -0, +hoopHeight);
	corners[3].set(-hoopWidth, -0, +hoopHeight);
	corners[4].set(-hoopWidth, +hoopLength, -hoopHeight);
	corners[5].set(+hoopWidth, +hoopLength, -hoopHeight);
	corners[6].set(+hoopWidth, +hoopLength, +hoopHeight);
	corners[7].set(-hoopWidth, +hoopLength, +hoopHeight);

	for (int i = 0; i < 8; i++) {
		corners[i].z += 3.5f / 16.0f;

		if (powered) {
			corners[i].y -= 1.5f / 16.0f;
			corners[i].z -= 2.6f / 16.0f;
			corners[i].xRot(0 * PI / 180);
		}
		else if (attached) {
			corners[i].y += 0.25f / 16.0f;
			corners[i].z -= 2.75f / 16.0f;
			corners[i].xRot(10 * PI / 180);
		}
		else {
			corners[i].xRot(50 * PI / 180);
		}

		if (dir == Direction::NORTH) {
			corners[i].yRot(0 * PI / 180);
		}
		if (dir == Direction::SOUTH) {
			corners[i].yRot(180 * PI / 180);
		}
		if (dir == Direction::WEST) {
			corners[i].yRot(90 * PI / 180);
		}
		if (dir == Direction::EAST) {
			corners[i].yRot(-90 * PI / 180);
		}

		corners[i].x += p.x + 0.5f;
		corners[i].y += p.y + 5.f / 16.0f;
		corners[i].z += p.z + 0.5f;
	}

	float hoopX0 = 5.f * scale;
	float hoopX1 = 11.f * scale;
	float hoopY0 = 3.f * scale;
	float hoopY1 = 9.f * scale;

	for (int i = 0; i < 6; i++) {
		if (i == 0) {
			c0 = corners[0];
			c1 = corners[1];
			c2 = corners[2];
			c3 = corners[3];
			u0 = tex.offsetWidthPixel(hoopX0);
			v0 = tex.offsetHeightPixel(hoopY0);
			u1 = tex.offsetWidthPixel(hoopX1);
			v1 = tex.offsetHeightPixel(hoopY1);
		}
		else if (i == 1) {
			c0 = corners[7];
			c1 = corners[6];
			c2 = corners[5];
			c3 = corners[4];
		}
		else if (i == 2) {
			c0 = corners[1];
			c1 = corners[0];
			c2 = corners[4];
			c3 = corners[5];
			u0 = tex.offsetWidthPixel(hoopX0);
			v0 = tex.offsetHeightPixel(hoopY0);
			u1 = tex.offsetWidthPixel(hoopX1);
			v1 = tex.offsetHeightPixel(hoopY0 + 2.f * scale);
		}
		else if (i == 3) {
			c0 = corners[2];
			c1 = corners[1];
			c2 = corners[5];
			c3 = corners[6];
		}
		else if (i == 4) {
			c0 = corners[3];
			c1 = corners[2];
			c2 = corners[6];
			c3 = corners[7];
		}
		else if (i == 5) {
			c0 = corners[0];
			c1 = corners[3];
			c2 = corners[7];
			c3 = corners[4];
		}
		mTessellator.vertexUV(c0.x, c0.y, c0.z, u0, v1);
		mTessellator.vertexUV(c1.x, c1.y, c1.z, u1, v1);
		mTessellator.vertexUV(c2.x, c2.y, c2.z, u1, v0);
		mTessellator.vertexUV(c3.x, c3.y, c3.z, u0, v0);
	}

	if (attached) {
		float hoopBottomY = corners[0].y;
		float width = 0.5f / 16.0f;
		float top = 0.5f - (width / 2);
		float bottom = top + width;
		float wireX0 = tex._u0;
		float wireY0 = tex.offsetHeightPixel(attached ? 2.0f * scale : 0.0f * scale);
		float wireX1 = tex._u1;
		float wireY1 = tex.offsetHeightPixel(attached ? 4.0f * scale : 2.0f * scale);
		float floating = 3.5f / 16.0f;

		mTessellator.color(.75f, .75f, .75f);

		if (dir == Direction::NORTH) {
			mTessellator.vertexUV(p.x + top, p.y + floating, p.z + 0.25f, wireX0, wireY0);
			mTessellator.vertexUV(p.x + bottom, p.y + floating, p.z + 0.25f, wireX0, wireY1);
			mTessellator.vertexUV(p.x + bottom, p.y + floating, (float)p.z, wireX1, wireY1);
			mTessellator.vertexUV(p.x + top, p.y + floating, (float)p.z, wireX1, wireY0);

			mTessellator.vertexUV(p.x + top, hoopBottomY, p.z + 0.5f, wireX0, wireY0);
			mTessellator.vertexUV(p.x + bottom, hoopBottomY, p.z + 0.5f, wireX0, wireY1);
			mTessellator.vertexUV(p.x + bottom, p.y + floating, p.z + 0.25f, wireX1, wireY1);
			mTessellator.vertexUV(p.x + top, p.y + floating, p.z + 0.25f, wireX1, wireY0);
		}
		else if (dir == Direction::SOUTH) {
			mTessellator.vertexUV(p.x + top, p.y + floating, p.z + 0.75f, wireX0, wireY0);
			mTessellator.vertexUV(p.x + bottom, p.y + floating, p.z + 0.75f, wireX0, wireY1);
			mTessellator.vertexUV(p.x + bottom, hoopBottomY, p.z + 0.5f, wireX1, wireY1);
			mTessellator.vertexUV(p.x + top, hoopBottomY, p.z + 0.5f, wireX1, wireY0);

			mTessellator.vertexUV(p.x + top, p.y + floating, p.z + 1.0f, wireX0, wireY0);
			mTessellator.vertexUV(p.x + bottom, p.y + floating, p.z + 1.0f, wireX0, wireY1);
			mTessellator.vertexUV(p.x + bottom, p.y + floating, p.z + 0.75f, wireX1, wireY1);
			mTessellator.vertexUV(p.x + top, p.y + floating, p.z + 0.75f, wireX1, wireY0);
		}
		else if (dir == Direction::WEST) {
			mTessellator.vertexUV((float)p.x, p.y + floating, p.z + bottom, wireX0, wireY1);
			mTessellator.vertexUV(p.x + 0.25f, p.y + floating, p.z + bottom, wireX1, wireY1);
			mTessellator.vertexUV(p.x + 0.25f, p.y + floating, p.z + top, wireX1, wireY0);
			mTessellator.vertexUV((float)p.x, p.y + floating, p.z + top, wireX0, wireY0);

			mTessellator.vertexUV(p.x + 0.25f, p.y + floating, p.z + bottom, wireX0, wireY1);
			mTessellator.vertexUV(p.x + 0.5f, hoopBottomY, p.z + bottom, wireX1, wireY1);
			mTessellator.vertexUV(p.x + 0.5f, hoopBottomY, p.z + top, wireX1, wireY0);
			mTessellator.vertexUV(p.x + 0.25f, p.y + floating, p.z + top, wireX0, wireY0);
		}
		else {
			mTessellator.vertexUV(p.x + 0.5f, hoopBottomY, p.z + bottom, wireX0, wireY1);
			mTessellator.vertexUV(p.x + 0.75f, p.y + floating, p.z + bottom, wireX1, wireY1);
			mTessellator.vertexUV(p.x + 0.75f, p.y + floating, p.z + top, wireX1, wireY0);
			mTessellator.vertexUV(p.x + 0.5f, hoopBottomY, p.z + top, wireX0, wireY0);

			mTessellator.vertexUV(p.x + 0.75f, p.y + floating, p.z + bottom, wireX0, wireY1);
			mTessellator.vertexUV(p.x + 1.f, p.y + floating, p.z + bottom, wireX1, wireY1);
			mTessellator.vertexUV(p.x + 1.f, p.y + floating, p.z + top, wireX1, wireY0);
			mTessellator.vertexUV(p.x + 0.75f, p.y + floating, p.z + top, wireX0, wireY0);
		}
	}

	return true;
}

bool BlockTessellator::tessellateTripwireInWorld(const Block& b, const BlockPos& p, int data) {
	const BlockGraphics& blockGraphics = *BlockGraphics::mBlocks[b.getId()];
	const TextureUVCoordinateSet& tex = mUseFixedTexture ? mFixedTexture : blockGraphics.getTexture(Facing::UP);
	bool attached = Block::mTripwire->getBlockState(BlockState::AttachedBit).getBool(data);

	_tex1(toUV(getLightColor(p)));
	mTessellator.color(1.0f, 1.0f, 1.0f);

	// Not all blocks are 16x16 anymore
	float scale = tex.pixelWidth() / 16.f;

	float wireX0 = tex._u0;
	float wireY0 = tex.offsetHeightPixel(attached ? 2.0f * scale : 0.0f * scale);
	float wireX1 = tex._u1;
	float wireY1 = tex.offsetHeightPixel(attached ? 4.0f * scale : 2.0f * scale);
	float floating = 3.5f / 16.0f;

	bool w = TripWireBlock::shouldConnectTo(*mRegion, p, data, Direction::WEST);
	bool e = TripWireBlock::shouldConnectTo(*mRegion, p, data, Direction::EAST);
	bool n = TripWireBlock::shouldConnectTo(*mRegion, p, data, Direction::NORTH);
	bool s = TripWireBlock::shouldConnectTo(*mRegion, p, data, Direction::SOUTH);

	float width = 0.5f / 16.0f;
	float top = 0.5f - (width / 2);
	float bottom = top + width;

	if (!n && !e && !s && !w) {
		n = true;
		s = true;
	}

	if (n) {
		mTessellator.vertexUV(p.x + top, p.y + floating, p.z + 0.25f, wireX0, wireY0);
		mTessellator.vertexUV(p.x + bottom, p.y + floating, p.z + 0.25f, wireX0, wireY1);
		mTessellator.vertexUV(p.x + bottom, p.y + floating, (float)p.z, wireX1, wireY1);
		mTessellator.vertexUV(p.x + top, p.y + floating, (float)p.z, wireX1, wireY0);

		mTessellator.vertexUV(p.x + top, p.y + floating, (float)p.z, wireX1, wireY0);
		mTessellator.vertexUV(p.x + bottom, p.y + floating, (float)p.z, wireX1, wireY1);
		mTessellator.vertexUV(p.x + bottom, p.y + floating, p.z + 0.25f, wireX0, wireY1);
		mTessellator.vertexUV(p.x + top, p.y + floating, p.z + 0.25f, wireX0, wireY0);
	}
	if (n || (s && !e && !w)) {
		mTessellator.vertexUV(p.x + top, p.y + floating, p.z + 0.5f, wireX0, wireY0);
		mTessellator.vertexUV(p.x + bottom, p.y + floating, p.z + 0.5f, wireX0, wireY1);
		mTessellator.vertexUV(p.x + bottom, p.y + floating, p.z + 0.25f, wireX1, wireY1);
		mTessellator.vertexUV(p.x + top, p.y + floating, p.z + 0.25f, wireX1, wireY0);

		mTessellator.vertexUV(p.x + top, p.y + floating, p.z + 0.25f, wireX1, wireY0);
		mTessellator.vertexUV(p.x + bottom, p.y + floating, p.z + 0.25f, wireX1, wireY1);
		mTessellator.vertexUV(p.x + bottom, p.y + floating, p.z + 0.5f, wireX0, wireY1);
		mTessellator.vertexUV(p.x + top, p.y + floating, p.z + 0.5f, wireX0, wireY0);
	}
	if (s || (n && !e && !w)) {
		mTessellator.vertexUV(p.x + top, p.y + floating, p.z + 0.75f, wireX0, wireY0);
		mTessellator.vertexUV(p.x + bottom, p.y + floating, p.z + 0.75f, wireX0, wireY1);
		mTessellator.vertexUV(p.x + bottom, p.y + floating, p.z + 0.5f, wireX1, wireY1);
		mTessellator.vertexUV(p.x + top, p.y + floating, p.z + 0.5f, wireX1, wireY0);

		mTessellator.vertexUV(p.x + top, p.y + floating, p.z + 0.5f, wireX1, wireY0);
		mTessellator.vertexUV(p.x + bottom, p.y + floating, p.z + 0.5f, wireX1, wireY1);
		mTessellator.vertexUV(p.x + bottom, p.y + floating, p.z + 0.75f, wireX0, wireY1);
		mTessellator.vertexUV(p.x + top, p.y + floating, p.z + 0.75f, wireX0, wireY0);
	}
	if (s) {
		mTessellator.vertexUV(p.x + top, p.y + floating, p.z + 1.f, wireX0, wireY0);
		mTessellator.vertexUV(p.x + bottom, p.y + floating, p.z + 1.f, wireX0, wireY1);
		mTessellator.vertexUV(p.x + bottom, p.y + floating, p.z + 0.75f, wireX1, wireY1);
		mTessellator.vertexUV(p.x + top, p.y + floating, p.z + 0.75f, wireX1, wireY0);

		mTessellator.vertexUV(p.x + top, p.y + floating, p.z + 0.75f, wireX1, wireY0);
		mTessellator.vertexUV(p.x + bottom, p.y + floating, p.z + 0.75f, wireX1, wireY1);
		mTessellator.vertexUV(p.x + bottom, p.y + floating, p.z + 1.f, wireX0, wireY1);
		mTessellator.vertexUV(p.x + top, p.y + floating, p.z + 1.f, wireX0, wireY0);
	}

	if (w) {
		mTessellator.vertexUV((float)p.x, p.y + floating, p.z + bottom, wireX0, wireY1);
		mTessellator.vertexUV(p.x + 0.25f, p.y + floating, p.z + bottom, wireX1, wireY1);
		mTessellator.vertexUV(p.x + 0.25f, p.y + floating, p.z + top, wireX1, wireY0);
		mTessellator.vertexUV((float)p.x, p.y + floating, p.z + top, wireX0, wireY0);

		mTessellator.vertexUV((float)p.x, p.y + floating, p.z + top, wireX0, wireY0);
		mTessellator.vertexUV(p.x + 0.25f, p.y + floating, p.z + top, wireX1, wireY0);
		mTessellator.vertexUV(p.x + 0.25f, p.y + floating, p.z + bottom, wireX1, wireY1);
		mTessellator.vertexUV((float)p.x, p.y + floating, p.z + bottom, wireX0, wireY1);
	}
	if (w || (e && !n && !s)) {
		mTessellator.vertexUV(p.x + 0.25f, p.y + floating, p.z + bottom, wireX0, wireY1);
		mTessellator.vertexUV(p.x + 0.5f, p.y + floating, p.z + bottom, wireX1, wireY1);
		mTessellator.vertexUV(p.x + 0.5f, p.y + floating, p.z + top, wireX1, wireY0);
		mTessellator.vertexUV(p.x + 0.25f, p.y + floating, p.z + top, wireX0, wireY0);

		mTessellator.vertexUV(p.x + 0.25f, p.y + floating, p.z + top, wireX0, wireY0);
		mTessellator.vertexUV(p.x + 0.5f, p.y + floating, p.z + top, wireX1, wireY0);
		mTessellator.vertexUV(p.x + 0.5f, p.y + floating, p.z + bottom, wireX1, wireY1);
		mTessellator.vertexUV(p.x + 0.25f, p.y + floating, p.z + bottom, wireX0, wireY1);
	}
	if (e || (w && !n && !s)) {
		mTessellator.vertexUV(p.x + 0.5f, p.y + floating, p.z + bottom, wireX0, wireY1);
		mTessellator.vertexUV(p.x + 0.75f, p.y + floating, p.z + bottom, wireX1, wireY1);
		mTessellator.vertexUV(p.x + 0.75f, p.y + floating, p.z + top, wireX1, wireY0);
		mTessellator.vertexUV(p.x + 0.5f, p.y + floating, p.z + top, wireX0, wireY0);

		mTessellator.vertexUV(p.x + 0.5f, p.y + floating, p.z + top, wireX0, wireY0);
		mTessellator.vertexUV(p.x + 0.75f, p.y + floating, p.z + top, wireX1, wireY0);
		mTessellator.vertexUV(p.x + 0.75f, p.y + floating, p.z + bottom, wireX1, wireY1);
		mTessellator.vertexUV(p.x + 0.5f, p.y + floating, p.z + bottom, wireX0, wireY1);
	}
	if (e) {
		mTessellator.vertexUV(p.x + 0.75f, p.y + floating, p.z + bottom, wireX0, wireY1);
		mTessellator.vertexUV(p.x + 1.f, p.y + floating, p.z + bottom, wireX1, wireY1);
		mTessellator.vertexUV(p.x + 1.f, p.y + floating, p.z + top, wireX1, wireY0);
		mTessellator.vertexUV(p.x + 0.75f, p.y + floating, p.z + top, wireX0, wireY0);

		mTessellator.vertexUV(p.x + 0.75f, p.y + floating, p.z + top, wireX0, wireY0);
		mTessellator.vertexUV(p.x + 1.f, p.y + floating, p.z + top, wireX1, wireY0);
		mTessellator.vertexUV(p.x + 1.f, p.y + floating, p.z + bottom, wireX1, wireY1);
		mTessellator.vertexUV(p.x + 0.75f, p.y + floating, p.z + bottom, wireX0, wireY1);
	}

	return true;
}

bool BlockTessellator::tessellateCauldronInWorld(const Block& b, const BlockPos& p, DataID data) {

	const BlockGraphics& blockGraphics = *BlockGraphics::mBlocks[b.getId()];
	static const FacingID TOP = Facing::UP;
	static const FacingID BOTTOM = Facing::DOWN;
	static const FacingID SIDE = Facing::NORTH;
	static const FacingID INSIDE = Facing::SOUTH;
	static const FacingID CAULDRON_WATER = Facing::WEST;
	static const FacingID STILL_WATER = Facing::EAST;

	if (mRenderingLayer == BlockRenderLayer::RENDERLAYER_OPAQUE) {
		auto& texSide   = blockGraphics.getTexture(SIDE);
		auto& texTop    = blockGraphics.getTexture(TOP);
		auto& texBottom = blockGraphics.getTexture(BOTTOM);
		auto& texInside = blockGraphics.getTexture(INSIDE);
		
		_tex1(toUV(getLightColor(p)));
		mTessellator.color(1.0f, 1.0f, 1.0f);

		mCurrentShape.set(Vec3(0, 3.0f / 16.0f, 0), Vec3(2.0f / 16.0f, 1.0f, 1.0f));
		renderFaceUp(b, p, texTop);
		renderFaceDown(b, p, texInside);
		renderNorth(b, p, texSide);
		renderSouth(b, p, texSide);
		renderEast(b, p, texSide);
		renderWest(b, p, texSide);

		mCurrentShape.set(Vec3(2.0f / 16.0f, 3.0f / 16.0f, 2.0f / 16.0f), Vec3(14.0f / 16.0f, 4.0f / 16.0f, 14.0f / 16.0f));
		renderFaceUp(b, p, texInside);
		renderFaceDown(b, p, texInside);
		renderNorth(b, p, texSide);
		renderSouth(b, p, texSide);
		renderEast(b, p, texSide);
		renderWest(b, p, texSide);

		mCurrentShape.set(Vec3(14.0f / 16.0f, 3.0f / 16.0f, 0.0f / 16.0f), Vec3(1.0f, 1.0f, 1.0f));
		renderFaceUp(b, p, texTop);
		renderFaceDown(b, p, texInside);
		renderNorth(b, p, texSide);
		renderSouth(b, p, texSide);
		renderEast(b, p, texSide);
		renderWest(b, p, texSide);

		mCurrentShape.set(Vec3(2.0f / 16.0f, 3.0f / 16.0f, 0), Vec3(14.0f / 16.0f, 1.0f, 2.0f / 16.0f));
		renderFaceUp(b, p, texTop);
		renderFaceDown(b, p, texInside);
		renderNorth(b, p, texSide);
		renderSouth(b, p, texSide);
		renderEast(b, p, texSide);
		renderWest(b, p, texSide);

		mCurrentShape.set(Vec3(2.0f / 16.0f, 3.0f / 16.0f, 14.0f / 16.0f), Vec3(14.0f / 16.0f, 1.0f, 1.0f));
		renderFaceUp(b, p, texTop);
		renderFaceDown(b, p, texInside);
		renderNorth(b, p, texSide);
		renderSouth(b, p, texSide);
		renderEast(b, p, texSide);
		renderWest(b, p, texSide);

		mCurrentShape.set(Vec3(0, 0, 0), Vec3(4.0f / 16.0f, 3.0f / 16.0f, 2.0f / 16.0f));
		renderFaceUp(b, p, texTop);
		renderFaceDown(b, p, texBottom);
		renderNorth(b, p, texSide);
		renderSouth(b, p, texSide);
		renderEast(b, p, texSide);
		renderWest(b, p, texSide);

		mCurrentShape.set(Vec3(0, 0, 2.0f / 16.0f), Vec3(2.0f / 16.0f, 3.0f / 16.0f, 4.0f / 16.0f));
		renderFaceUp(b, p, texTop);
		renderFaceDown(b, p, texBottom);
		renderNorth(b, p, texSide);
		renderSouth(b, p, texSide);
		renderEast(b, p, texSide);
		renderWest(b, p, texSide);

		mCurrentShape.set(Vec3(12.0f / 16.0f, 0, 0), Vec3(1.0f, 3.0f / 16.0f, 2.0f / 16.0f));
		renderFaceUp(b, p, texTop);
		renderFaceDown(b, p, texBottom);
		renderNorth(b, p, texSide);
		renderSouth(b, p, texSide);
		renderEast(b, p, texSide);
		renderWest(b, p, texSide);

		mCurrentShape.set(Vec3(14.0f / 16.0f, 0, 2.0f / 16.0f), Vec3(1.0f, 3.0f / 16.0f, 4.0f / 16.0f));
		renderFaceUp(b, p, texTop);
		renderFaceDown(b, p, texBottom);
		renderNorth(b, p, texSide);
		renderSouth(b, p, texSide);
		renderEast(b, p, texSide);
		renderWest(b, p, texSide);

		mCurrentShape.set(Vec3(0, 0, 14.0f / 16.0f), Vec3(4.0f / 16.0f, 3.0f / 16.0f, 1.0f));
		renderFaceUp(b, p, texTop);
		renderFaceDown(b, p, texBottom);
		renderNorth(b, p, texSide);
		renderSouth(b, p, texSide);
		renderEast(b, p, texSide);
		renderWest(b, p, texSide);

		mCurrentShape.set(Vec3(0, 0, 12.0f / 16.0f), Vec3(2.0f / 16.0f, 3.0f / 16.0f, 14.0f / 16.0f));
		renderFaceUp(b, p, texTop);
		renderFaceDown(b, p, texBottom);
		renderNorth(b, p, texSide);
		renderSouth(b, p, texSide);
		renderEast(b, p, texSide);
		renderWest(b, p, texSide);

		mCurrentShape.set(Vec3(12.0f / 16.0f, 0, 14.0f / 16.0f), Vec3(1.0f, 3.0f / 16.0f, 1.0f));
		renderFaceUp(b, p, texTop);
		renderFaceDown(b, p, texBottom);
		renderNorth(b, p, texSide);
		renderSouth(b, p, texSide);
		renderEast(b, p, texSide);
		renderWest(b, p, texSide);

		mCurrentShape.set(Vec3(14.0f / 16.0f, 0, 12.0f / 16.0f), Vec3(1.0f, 3.0f / 16.0f, 14.0f / 16.0f));
		renderFaceUp(b, p, texTop);
		renderFaceDown(b, p, texBottom);
		renderNorth(b, p, texSide);
		renderSouth(b, p, texSide);
		renderEast(b, p, texSide);
		renderWest(b, p, texSide);
	}

	if (data > 0) {
		int requiredRenderLayer = BlockRenderLayer::RENDERLAYER_OPAQUE;

		//TODO: rherlitz
// 		CauldronBlockEntity* cauldronEntity = nullptr;
// 		if(mRegion) {
// 			// region will be null if this block can be pushed.
// 			cauldronEntity = static_cast<CauldronBlockEntity*>(mRegion->getBlockEntity(p));
// 		}
// 
 		int textureIndex = CAULDRON_WATER;
// 		// Don't add the water into the block tessellation if there's a potion in the cauldron as that will be drawn by the BlockEntity
// 		if (cauldronEntity != nullptr) {
// 			if (cauldronEntity->getPotionId() >= 0) {
// 				mTessellator.color(cauldronEntity->getPotionColor());
// 			}
// 			else if (cauldronEntity->hasCustomColor()) {
// 				mTessellator.color(cauldronEntity->getCustomColor());
// 			}
// 			else {
// 				textureIndex = STILL_WATER;
// 				mTessellator.color(CauldronBlockEntity::WATER_COLOR);
// 				requiredRenderLayer = BlockRenderLayer::RENDERLAYER_BLEND;
// 			}
// 		}
// 		else {
// 			textureIndex = STILL_WATER;
// 			mTessellator.color(CauldronBlockEntity::WATER_COLOR);
// 			requiredRenderLayer = BlockRenderLayer::RENDERLAYER_BLEND;
// 		}

		if (requiredRenderLayer == mRenderingLayer) {
			auto& texWater = blockGraphics.getTexture(textureIndex);
		
			float height = 3.0f / 16.0f + CauldronBlock::getWaterLevel(data) * 2.0f / 16.0f;
			mCurrentShape.set(Vec3(2.0f / 16.0f, height, 2.0f / 16.0f), Vec3(14.0f / 16.0f, height, 14.0f / 16.0f));
			renderFaceUp(b, p, texWater);
		}
	}

	return true;
}

bool BlockTessellator::tessellateRepeaterInWorld(const Block& block, const BlockPos& p, int data) {
	int dir = Block::mPoweredRepeater->getBlockState(BlockState::Direction).get<int>(data);
	int delay = Block::mPoweredRepeater->getBlockState(BlockState::RepeaterDelay).get<int>(data);

	_tex1(toUV(getLightColor(p)));
	mTessellator.color(1.0f, 1.0f, 1.0f);

	float h = -3.0f / 16.0f;
	bool hasLockSignal = static_cast<const RepeaterBlock&>(block).isLocked(*mRegion, p, data);
	float transmitterX = 0;
	float transmitterZ = 0;
	float receiverX = 0;
	float receiverZ = 0;

	switch (dir) {
	case Direction::SOUTH:
		receiverZ = -5.0f / 16.0f;
		transmitterZ = RepeaterBlock::DELAY_RENDER_OFFSETS[delay];
		break;
	case Direction::NORTH:
		receiverZ = 5.0f / 16.0f;
		transmitterZ = -RepeaterBlock::DELAY_RENDER_OFFSETS[delay];
		break;
	case Direction::EAST:
		receiverX = -5.0f / 16.0f;
		transmitterX = RepeaterBlock::DELAY_RENDER_OFFSETS[delay];
		break;
	case Direction::WEST:
		receiverX = 5.0f / 16.0f;
		transmitterX = -RepeaterBlock::DELAY_RENDER_OFFSETS[delay];
		break;
	}

	// render transmitter
	if (!hasLockSignal) {
		tessellateTorch(block, Vec3((float)p.x + transmitterX, (float)p.y + h, (float)p.z + transmitterZ), 0, 0);
	} else {
		const TextureUVCoordinateSet& lockTex = _getTexture(*Block::mBedrock, Facing::UP, data);
		mFixedTexture = lockTex;
		mUseFixedTexture = true;

		float west = 2.0f;
		float east = 14.0f;
		float north = 7.0f;
		float south = 9.0f;

		switch (dir) {
		case Direction::SOUTH:
		case Direction::NORTH:
			break;
		case Direction::EAST:
		case Direction::WEST:
			west = 7.0f;
			east = 9.0f;
			north = 2.0f;
			south = 14.0f;
			break;
		}

		mCurrentShape.set(west / 16.0f + transmitterX, 2.0f / 16.0f, north / 16.0f + transmitterZ, east / 16.0f + transmitterX, 4.0f / 16.0f, south / 16.0f + transmitterZ);
		float u0 = lockTex.offsetWidthPixel(west);
		float v0 = lockTex.offsetHeightPixel(north);
		float u1 = lockTex.offsetWidthPixel(east);
		float v1 = lockTex.offsetHeightPixel(south);
		float yOffset = (4.0f / 16.0f) + 0.0005f;
		mTessellator.vertexUV(p.x + west / 16.0f + transmitterX, p.y + yOffset, p.z + north / 16.0f + transmitterZ, u0, v0);
		mTessellator.vertexUV(p.x + west / 16.0f + transmitterX, p.y + yOffset, p.z + south / 16.0f + transmitterZ, u0, v1);
		mTessellator.vertexUV(p.x + east / 16.0f + transmitterX, p.y + yOffset, p.z + south / 16.0f + transmitterZ, u1, v1);
		mTessellator.vertexUV(p.x + east / 16.0f + transmitterX, p.y + yOffset, p.z + north / 16.0f + transmitterZ, u1, v0);
		tessellateBlockInWorld(block, p, data);
		mCurrentShape.set(0, 0, 0, 1, 2.0f / 16.0f, 1);
		mFixedTexture = TextureUVCoordinateSet();
		mUseFixedTexture = false;
	}

	_tex1(toUV(getLightColor(p)));
	mTessellator.color(1.0f, 1.0f, 1.0f);

	//// render receiver
	tessellateTorch(block, Vec3((float)p.x + receiverX, (float)p.y + h, (float)p.z + receiverZ), 0, 0);

	// render floor
	tessellateDiodeInWorld(block, p, dir);

	return true;
}

bool BlockTessellator::tessellateComparatorInWorld(const Block& block, const BlockPos& p, int data) {
	_tex1(toUV(getLightColor(p)));
	mTessellator.color(1.0f, 1.0f, 1.0f);

	int dir = Block::mPoweredComparator->getBlockState(BlockState::Direction).get<int>(data);
	double extenderX = 0;
	double extenderY = -6.0f / 16.0f;
	double extenderZ = 0;
	double inputXStep = 0;
	double inputZStep = 0;
	TextureUVCoordinateSet extenderTex;

	static const FacingID TORCH = Facing::DOWN;
	if (static_cast<const ComparatorBlock&>(block).isSubtractMode(*mRegion, p)) {
		extenderTex = BlockGraphics::mBlocks[block.getId()]->getTexture(TORCH, 1);
		extenderY += 1 / 16.0f;
	} else {
		extenderTex = BlockGraphics::mBlocks[block.getId()]->getTexture(TORCH, 0);
	}

	switch (dir) {
	case Direction::SOUTH:
		extenderZ = -5.0f / 16.0f;
		inputZStep = 1;
		break;
	case Direction::NORTH:
		extenderZ = 5.0f / 16.0f;
		inputZStep = -1;
		break;
	case Direction::EAST:
		extenderX = -5.0f / 16.0f;
		inputXStep = 1;
		break;
	case Direction::WEST:
		extenderX = 5.0f / 16.0f;
		inputXStep = -1;
		break;
	}

	// Render the two input torches
	tessellateTorch(block, Vec3(p.x + (float)(4 / 16.0f * inputXStep) + (float)(3 / 16.0f * inputZStep), p.y - 3 / 16.0f, p.z + (float)(4 / 16.0f * inputZStep) + (float)(3 / 16.0f * inputXStep)), 0, 0);
	tessellateTorch(block, Vec3(p.x + (float)(4 / 16.0f * inputXStep) + (float)(-3 / 16.0f * inputZStep), p.y - 3 / 16.0f, p.z + (float)(4 / 16.0f * inputZStep) + (float)(-3 / 16.0f * inputXStep)), 0, 0);

	mFixedTexture = extenderTex;
	mUseFixedTexture = true;
	tessellateTorch(block, Vec3((float)(p.x + extenderX), (float)(p.y + extenderY), (float)(p.z + extenderZ)), 0, 0);
	mFixedTexture = TextureUVCoordinateSet();
	mUseFixedTexture = false;

	tessellateDiodeInWorld(block, p, dir);

	return true;
}

bool BlockTessellator::tessellateDiodeInWorld(const Block& block, const BlockPos& p, int data) {
	if (data == Direction::NORTH) {
		mFlipFace[Facing::UP] = FLIP_180;
	}
	else if (data == Direction::WEST) {
		mFlipFace[Facing::UP] = FLIP_CW;
	}
	else if (data == Direction::EAST) {
		mFlipFace[Facing::UP] = FLIP_CCW;
	}
	
	tessellateBlockInWorld(block, p, data);
	return true;
}

bool BlockTessellator::tessellatePistonInWorld(const Block& block, const BlockPos& pos, int data) {
	//mTessellator.color(block.getColor(data));
	
	FacingID facing = data & 0x07;
	switch(facing) {
	case Facing::UP:
		mCurrentShape.set(0.0f, 0.0f, 0.0f, 1.0f, 12.0f/16.0f, 1.0f);
		break;
	case Facing::DOWN:
		mFlipFace[Facing::NORTH] = FLIP_180;
		mFlipFace[Facing::SOUTH] = FLIP_180;
		mFlipFace[Facing::EAST] = FLIP_180;
		mFlipFace[Facing::WEST] = FLIP_180;
		mCurrentShape.set(0.0f, 4.0f/16.0f, 0.0f, 1.0f, 1.0f, 1.0f);
		break;
	case Facing::NORTH:
		mFlipFace[Facing::UP] = FLIP_180;
		mFlipFace[Facing::DOWN] = FLIP_180;
		mFlipFace[Facing::EAST] = FLIP_CCW;
		mFlipFace[Facing::WEST] = FLIP_CW;
		mCurrentShape.set(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 12.0f / 16.0f);
		break;
	case Facing::SOUTH:
		mFlipFace[Facing::EAST] = FLIP_CW;
		mFlipFace[Facing::WEST] = FLIP_CCW;
		mCurrentShape.set(0.0f, 0.0f, 4.0f / 16.0f, 1.0f, 1.0f, 1.0f);
		break;
	case Facing::EAST:
		mFlipFace[Facing::UP] = FLIP_CCW;
		mFlipFace[Facing::DOWN] = FLIP_CW;
		mFlipFace[Facing::NORTH] = FLIP_CW;
		mFlipFace[Facing::SOUTH] = FLIP_CCW;
		mCurrentShape.set(4.0f / 16.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
		break;
	case Facing::WEST:
		mFlipFace[Facing::UP] = FLIP_CW;
		mFlipFace[Facing::DOWN] = FLIP_CCW;
		mFlipFace[Facing::NORTH] = FLIP_CCW;
		mFlipFace[Facing::SOUTH] = FLIP_CW;
		mCurrentShape.set(0.0f, 0.0f, 0.0f, 12.0f / 16.0f, 1.0f, 1.0f);
		break;
	}

	tessellateBlockInWorld(block, pos, data);
	mUseFixedTexture = false;

	return true; 
}

bool BlockTessellator::tessellateChorusFlowerInWorld(const Block& block, const BlockPos& pos, int data, bool render) {
	// Special hardcoded rendering of pure justice 
	for (int i = 0; i < 3; ++i) {
		Vec3 offset = Vec3(ChorusPlantBlock::PLANT_ITEM_DIMENSION);
		offset[i] = 0; // This will make each of the 3 draw blocks full length on just one axis 
		mCurrentShape.set(offset, Vec3(1.0f) - offset);
		if (render) {
			_renderAllFaces(block, Vec3::ZERO, data);
		}
		else {
			tessellateBlockInWorld(block, pos, data);
		}
	}
	return true;
}

bool BlockTessellator::tessellateChorusPlantInWorld(const ChorusPlantBlock& block, const BlockPos& pos, int data) {
	// Check what directions are connecting 
	bool above = block.connectsTo(*mRegion, pos, pos.above());
	bool below = block.connectsTo(*mRegion, pos, pos.below());
	bool north = block.connectsTo(*mRegion, pos, pos.north());
	bool east = block.connectsTo(*mRegion, pos, pos.east());
	bool south = block.connectsTo(*mRegion, pos, pos.south());
	bool west = block.connectsTo(*mRegion, pos, pos.west());
	static const float OFFSET = 4.0f / 16.0f;
	static const float OFFSET_THIN = 3.0f / 16.0f;
	////////////////////////////////////////////////////////////////////////// Cardinal directions 
	int switchValue = abs((pos.x + pos.y + pos.z) % 4); // Decides what end caps should be created surrounding the plant's center 
	bool drawCenter = false;
	// This is going to be a pain in my blocky Minecraft butt 
	if (north) {
		_setShapeAndTessellate(Vec3(.5f - OFFSET, .5f - OFFSET, 0), Vec3(.5f + OFFSET, .5f + OFFSET, .5f - OFFSET), block, pos, data);
	}
	else {
		// Determine what extra sides to draw if any 
		switch (switchValue) {
		case 1:
		case 3:
			_setShapeAndTessellate(Vec3(.5f - OFFSET, .5f - OFFSET, 3.0f / 16.0f), Vec3(.5f + OFFSET, .5f + OFFSET, 4.0f / 16.0f), block, pos, data);
			break;
		case 2:
			_setShapeAndTessellate(Vec3(.5f - OFFSET_THIN, .5f - OFFSET_THIN, 2.0f / 16.0f), Vec3(.5f + OFFSET_THIN, .5f + OFFSET_THIN, 4.0f / 16.0f), block, pos, data);
		default: // If OFFSET_THIN is used or the default is hit, the center of the plant should be drawn 
			drawCenter = true;
		}
	}
	// 90 deg Y rotation of North 
	if (east) {
		_setShapeAndTessellate(Vec3(.5f + OFFSET, .5f - OFFSET, .5f - OFFSET), Vec3(1.0f, .5f + OFFSET, .5f + OFFSET), block, pos, data);
	}
	else {
		// Determine what extra sides to draw if any 
		switch (switchValue) {
		case 0:
		case 2:
			_setShapeAndTessellate(Vec3(12.0f / 16.0f, .5f - OFFSET, .5f - OFFSET), Vec3(13.0f / 16.0f, .5f + OFFSET, .5f + OFFSET), block, pos, data);
			break;
		case 1:
			_setShapeAndTessellate(Vec3(12.0f / 16.0f, .5f - OFFSET_THIN, .5f - OFFSET_THIN), Vec3(14.0f / 16.0f, .5f + OFFSET_THIN, .5f + OFFSET_THIN), block, pos, data);
		default: // If OFFSET_THIN is used or the default is hit, the center of the plant should be drawn 
			drawCenter = true;
		}
	}
	// 180 deg Y rotation of North 
	if (south) {
		_setShapeAndTessellate(Vec3(.5f - OFFSET, .5f - OFFSET, .5f + OFFSET), Vec3(.5f + OFFSET, .5f + OFFSET, 1.0f), block, pos, data);
	}
	else {
		// Determine what extra sides to draw if any 
		switch (switchValue) {
		case 1:
		case 3:
			_setShapeAndTessellate(Vec3(.5f - OFFSET, .5f - OFFSET, 12.0f / 16.0f), Vec3(.5f + OFFSET, .5f + OFFSET, 13.0f / 16.0f), block, pos, data);
			break;
		case 0:
			_setShapeAndTessellate(Vec3(.5f - OFFSET_THIN, .5f - OFFSET_THIN, 12.0f / 16.0f), Vec3(.5f + OFFSET_THIN, .5f + OFFSET_THIN, 14.0f / 16.0f), block, pos, data);
		default: // If OFFSET_THIN is used or the default is hit, the center of the plant should be drawn 
			drawCenter = true;
		}
	}
	// 270 deg Y rotation of North 
	if (west) {
		_setShapeAndTessellate(Vec3(0, .5f - OFFSET, .5f - OFFSET), Vec3(.5f - OFFSET, .5f + OFFSET, .5f + OFFSET), block, pos, data);
	}
	else {
		// Determine what extra sides to draw if any 
		switch (switchValue) {
		case 0:
		case 2:
			_setShapeAndTessellate(Vec3(3.0f / 16.0f, .5f - OFFSET, .5f - OFFSET), Vec3(4.0f / 16.0f, .5f + OFFSET, .5f + OFFSET), block, pos, data);
			break;
		case 3:
			_setShapeAndTessellate(Vec3(2.0f / 16.0f, .5f - OFFSET_THIN, .5f - OFFSET_THIN), Vec3(4.0f / 16.0f, .5f + OFFSET_THIN, .5f + OFFSET_THIN), block, pos, data);
		default: // If OFFSET_THIN is used or the default is hit, the center of the plant should be drawn 
			drawCenter = true;
		}
	}
	
	////////////////////////////////////////////////////////////////////////// Up/Down 
	// 270 deg X rotation of North 
	if (above) {
		_setShapeAndTessellate(Vec3(.5f - OFFSET, .5f + OFFSET, .5f - OFFSET), Vec3(.5f + OFFSET, 1.0f, .5f + OFFSET), block, pos, data);
	}
	else {
		// Determine what extra sides to draw if any 
		switch (switchValue) {
		case 1:
		case 2:
			_setShapeAndTessellate(Vec3(.5f - OFFSET, 12.0f / 16.0f, .5f - OFFSET), Vec3(.5f + OFFSET, 13.0f / 16.0f, .5f + OFFSET), block, pos, data);
			break;
		case 3:
			_setShapeAndTessellate(Vec3(.5f - OFFSET_THIN, 12.0f / 16.0f, .5f - OFFSET_THIN), Vec3(.5f + OFFSET_THIN, 14.0f / 16.0f, .5f + OFFSET_THIN), block, pos, data);
		default: // If OFFSET_THIN is used or the default is hit, the center of the plant should be drawn 
			drawCenter = true;
		}
	}
	// 90 deg X rotation of North 
	if (below) {
		_setShapeAndTessellate(Vec3(.5f - OFFSET, 0, .5f - OFFSET), Vec3(.5f + OFFSET, .5f - OFFSET, .5f + OFFSET), block, pos, data);
	}
	else {
		// Determine what extra sides to draw if any 
		switch (switchValue) {
		case 0:
		case 2:
			_setShapeAndTessellate(Vec3(.5f - OFFSET, 3.0f / 16.0f, .5f - OFFSET), Vec3(.5f + OFFSET, 4.0f / 16.0f, .5f + OFFSET), block, pos, data);
			break;
		case 1:
			_setShapeAndTessellate(Vec3(.5f - OFFSET_THIN, 2.0f / 16.0f, .5f - OFFSET_THIN), Vec3(.5f + OFFSET_THIN, 4.0f / 16.0f, .5f + OFFSET_THIN), block, pos, data);
		default: // If OFFSET_THIN is used or the default is hit, the center of the plant should be drawn 
			drawCenter = true;
		}
	}
	
	////////////////////////////////////////////////////////////////////////// Main 
	if (drawCenter) {
		// The center will only be drawn if it can be seen (based on what happened above) 
		_setShapeAndTessellate(Vec3(.5f - OFFSET, .5f - OFFSET, .5f - OFFSET), Vec3(.5f + OFFSET, .5f + OFFSET, .5f + OFFSET), block, pos, data);
	}
	return true;
}

bool BlockTessellator::tessellateStructureVoidInWorld(const Block& block, const BlockPos& pos, int data, bool render) {
	mCurrentShape.set(Vec3(5.0f / 16.0f), Vec3(11.0f / 16.0f));
	if (render) {
		const BlockGraphics& blockGraphics = *BlockGraphics::mBlocks[block.getId()];
		renderFaceUp(block, pos, blockGraphics.getTexture(Facing::UP, data));
		renderFaceDown(block, pos, blockGraphics.getTexture(Facing::DOWN, data));
		renderNorth(block, pos, blockGraphics.getTexture(Facing::NORTH, data));
		renderSouth(block, pos, blockGraphics.getTexture(Facing::SOUTH, data));
		renderWest(block, pos, blockGraphics.getTexture(Facing::WEST, data));
		renderEast(block, pos, blockGraphics.getTexture(Facing::EAST, data));
	}
	else {
		tessellateBlockInWorld(block, pos, data);
	}

	return true;
}

bool BlockTessellator::tessellateHopperInWorld(const Block& block, const BlockPos& pos, int data) {
	_tex1(toUV(getLightColor(pos)));
	int col = block.getColor(data);
	float r = ((col >> 16) & 0xff) / 255.0f;
	float g = ((col >> 8) & 0xff) / 255.0f;
	float b = ((col)& 0xff) / 255.0f;

	mTessellator.color(r, g, b);

	return tessellateHopperInWorld(block, pos, data, false);
}

bool BlockTessellator::tessellateHopperInWorld(const Block& block, const BlockPos& pos, int data, bool render) {
	
	static const FacingID OUTSIDE = Facing::NORTH;
	static const FacingID INSIDE = Facing::DOWN;
	const BlockGraphics& blockGraphics = *BlockGraphics::mBlocks[block.getId()];
	int variant = block.getVariant(data);
	const TextureUVCoordinateSet& hopperTex = blockGraphics.getTexture(OUTSIDE, 0);
	const TextureUVCoordinateSet& bottomTex = blockGraphics.getTexture(INSIDE, 0);

	float bottom = 10.0f / 16.0f;
	float cWidth = 1.98f / 16.0f;

	// When tessellating in chuck, offset by block position
	Vec3 p = render ? Vec3::ZERO : Vec3(pos);

	// Render inside top box, do this first without ambient occlusion

	// Render hopper insides by shifting the block shape so it's outer side 
	// is aligned where the inner hopper side should be drawn

	mCurrentShape.set(0, bottom, cWidth, 1.0f, 1.0f, 1.0f - cWidth);
	renderEast(block, p + Vec3(-1.0f + cWidth, 0, 0), hopperTex);
	renderWest(block, p + Vec3(1.0f - cWidth, 0, 0), hopperTex);

	mCurrentShape.set(cWidth, bottom, 0, 1.0f - cWidth, 1.0f, 1.0f);
	renderSouth(block, p + Vec3(0, 0, -1.0f + cWidth), hopperTex);
	renderNorth(block, p + Vec3(0, 0, 1.0f - cWidth), hopperTex);

	mCurrentShape.set(cWidth, bottom, cWidth, 1.0f - cWidth, 1.0f, 1.0f - cWidth);
	renderFaceUp(block, p + Vec3(0, -1.0f + bottom, 0), bottomTex);


	// render outside top box
	mCurrentShape.set(0.0f, bottom, 0.0f, 1.0f, 1.0f, 1.0f);
	if(render) {
		renderFaceDown(block, Vec3::ZERO, blockGraphics.getTexture(Facing::DOWN, variant));
		renderFaceUp(block, Vec3::ZERO, blockGraphics.getTexture(Facing::UP, variant));
		renderNorth(block, Vec3::ZERO, blockGraphics.getTexture(Facing::NORTH, variant));
		renderSouth(block, Vec3::ZERO, blockGraphics.getTexture(Facing::SOUTH, variant));
		renderWest(block, Vec3::ZERO, blockGraphics.getTexture(Facing::WEST, variant));
		renderEast(block, Vec3::ZERO, blockGraphics.getTexture(Facing::EAST, variant));
	} else {
		tessellateBlockInWorld(block, pos, data);
	}
	
	// render bottom box
	mFixedTexture = hopperTex;
	float inset = 4.0f / 16.0f;
	float lboxy0 = 4.0f / 16.0f;
	float lboxy1 = bottom;
	mCurrentShape.set(inset, lboxy0, inset, 1.0f - inset, lboxy1 - .003f, 1.0f - inset);

	if (render) {
		renderEast(block, Vec3::ZERO, hopperTex);
		renderWest(block, Vec3::ZERO, hopperTex);
		renderSouth(block, Vec3::ZERO, hopperTex);
		renderNorth(block, Vec3::ZERO, hopperTex);
		renderFaceUp(block, Vec3::ZERO, hopperTex);
		renderFaceDown(block, Vec3::ZERO, hopperTex);
	} else {
		tessellateBlockInWorld(block, pos, data);
	}

	if (!render) {
		// render pipe
		float pipe = 6.0f / 16.0f;
		float pipeW = 4.0f / 16.0f;
		mFixedTexture = hopperTex;
		int facing = HopperBlock::getAttachedFace(data);

		// down
		if (facing == Facing::DOWN) {
			mCurrentShape.set(pipe, 0, pipe, 1.0f - pipe, 4.0f / 16.0f, 1.0f - pipe);
			tessellateBlockInWorld(block, pos, data);
		}
		// north
		if (facing == Facing::NORTH) {
			mCurrentShape.set(pipe, lboxy0, 0, 1.0f - pipe, lboxy0 + pipeW, inset);
			tessellateBlockInWorld(block, pos, data);
		}
		// south
		if (facing == Facing::SOUTH) {
			mCurrentShape.set(pipe, lboxy0, 1.0f - inset, 1.0f - pipe, lboxy0 + pipeW, 1.0f);
			tessellateBlockInWorld(block, pos, data);
		}
		// west
		if (facing == Facing::WEST) {
			mCurrentShape.set(0, lboxy0, pipe, inset, lboxy0 + pipeW, 1.0f - pipe);
			tessellateBlockInWorld(block, pos, data);
		}
		// east
		if (facing == Facing::EAST) {
			mCurrentShape.set(1.0f - inset, lboxy0, pipe, 1.0f, lboxy0 + pipeW, 1.0f - pipe);
			tessellateBlockInWorld(block, pos, data);
		}
	}

	mFixedTexture = TextureUVCoordinateSet();

	return true;
}

bool BlockTessellator::tessellateDragonEgg(const Block& block, const BlockPos& pos, int data, bool render) {
	static const AABB shapeSet[] = {
		AABB(Vec3(6.0f, 15.0f, 6.0f) / 16.0f, Vec3(10.0f, 16.0f, 10.0f) / 16.0f),
		AABB(Vec3(5.0f, 14.0f, 5.0f) / 16.0f, Vec3(11.0f, 15.0f, 11.0f) / 16.0f),
		AABB(Vec3(5.0f, 13.0f, 5.0f) / 16.0f, Vec3(11.0f, 14.0f, 11.0f) / 16.0f),
		AABB(Vec3(3.0f, 11.0f, 3.0f) / 16.0f, Vec3(13.0f, 13.0f, 13.0f) / 16.0f),
		AABB(Vec3(2.0f, 8.0f, 2.0f) / 16.0f, Vec3(14.0f, 11.0f, 14.0f) / 16.0f),
		AABB(Vec3(1.0f, 3.0f, 1.0f) / 16.0f, Vec3(15.0f, 8.0f, 15.0f) / 16.0f),
		AABB(Vec3(2.0f, 1.0f, 2.0f) / 16.0f, Vec3(14.0f, 3.0f, 14.0f) / 16.0f),
		AABB(Vec3(3.0f, 0.0f, 3.0f) / 16.0f, Vec3(13.0f, 1.0f, 13.0f) / 16.0f)
	};

	// Draw all shapes that make up this object 
	for (auto &currentShape : shapeSet) {
		mCurrentShape.set(currentShape);
		if (render) {
			_renderAllFaces(block, pos, data);
		}
		else {
			tessellateBlockInWorld(block, pos, data);
		}
	}

	return true;
}

bool BlockTessellator::tessellateSlimeBlockInWorld(const Block& block, const BlockPos& pos, int data) {
	if (mRenderingLayer == BlockRenderLayer::RENDERLAYER_OPAQUE) {
		const float innerShape = 3.0f / 16.0f;
		mCurrentShape.set(Vec3(innerShape), Vec3::ONE - Vec3(innerShape));
		tessellateBlockInWorld(block, pos, data);
	} else if (mRenderingLayer == BlockRenderLayer::RENDERLAYER_BLEND) {
		mCurrentShape.set(Vec3::ZERO, Vec3::ONE);
		tessellateBlockInWorld(block, pos, data);
	}

	return true;
}

bool BlockTessellator::tessellateBeaconInWorld(const Block& block, const BlockPos& pos, int data) {
	if (mRenderingLayer == BlockRenderLayer::RENDERLAYER_ALPHATEST_SINGLE_SIDE) {
		auto& texBase = _getTexture(*Block::mObsidian, Facing::UP, data);// block.getTextureNum(0);
		auto& texCore = _getTexture(block, Facing::UP, data);//block.getTextureNum(1);
		auto& texShell = _getTexture(*Block::mGlass, Facing::UP, data);//block.getTextureNum(2);

		mTessellator.color(1.0f, 1.0f, 1.0f);

		// beacon base
		const float fDiv = 1.0f / 16.0f;
		mCurrentShape.set(Vec3(2.0f * fDiv, 0.1f * fDiv, 2.0f * fDiv), Vec3(14.0f * fDiv, 3.0f * fDiv, 14.0f * fDiv));
		renderAll(block, pos, texBase);

		// beacon core
		mCurrentShape.set(Vec3(3.0f * fDiv, 3.0f * fDiv, 3.0f * fDiv), Vec3(13.0f * fDiv, 14.0f * fDiv, 13.0f * fDiv));
		renderAll(block, pos, texCore);

		// beacon outer shell
		mCurrentShape.set(Vec3::ZERO, Vec3::ONE);
		renderAll(block, pos, texShell);
	}

	return true;
}

void BlockTessellator::tessellateBeacon(const Block& block, float lightMultiplier, const Vec3& vOffset /* = Vec3::ZERO */) {
	const float fDiv = 1.0f / 16.0f;
	const Vec3 vShapeExtents[3][2] = {
		{ Vec3(2.0f * fDiv, 0.1f * fDiv, 2.0f * fDiv), Vec3(14.0f * fDiv, 3.0f * fDiv, 14.0f * fDiv) },
		{ Vec3(3.0f * fDiv, 3.0f * fDiv, 3.0f * fDiv), Vec3(13.0f * fDiv, 14.0f * fDiv, 13.0f * fDiv) },
		{ Vec3::ZERO, Vec3::ONE }
	};

	mTessellator.addOffset(vOffset);
	mTessellator.color(lightMultiplier, lightMultiplier, lightMultiplier);
	std::vector<BlockGraphics *> blockGraphics = {
		BlockGraphics::mBlocks[Block::mObsidian->getId()],
		BlockGraphics::mBlocks[block.getId()],
		BlockGraphics::mBlocks[Block::mGlass->getId()]
	};

	for (int i = 0; i < 3; ++i) {
		mCurrentShape.set(vShapeExtents[i][0], vShapeExtents[i][1]);
		renderFaceUp(block, Vec3::ZERO, blockGraphics[i]->getTexture(Facing::UP));
		renderSouth(block, Vec3::ZERO, blockGraphics[i]->getTexture(Facing::SOUTH));
		renderWest(block, Vec3::ZERO, blockGraphics[i]->getTexture(Facing::WEST));
	}
}

bool BlockTessellator::tessellateEndRodInWorld(const Block& b, const BlockPos& p, DataID data) {
	DataID dir = data;
	const BlockGraphics& blockGraphics = *BlockGraphics::mBlocks[b.getId()];

	if (mRenderingLayer == BlockRenderLayer::RENDERLAYER_OPAQUE) {
		auto& tex = blockGraphics.getTexture(Facing::DOWN);
		mTessellator.color(1.0f, 1.0f, 1.0f);

		// Rotate based on direction of placement
		if (dir == Facing::UP) {
			tessellateEndRodUp(p, tex);
		}
		else if (dir == Facing::DOWN) {
			tessellateEndRodDown(p, tex);
		}
		else if (dir == Facing::NORTH) {
			tessellateEndRodNorth(p, tex);
		}
		else if (dir == Facing::SOUTH) {
			tessellateEndRodSouth(p, tex);
		}
		else if (dir == Facing::WEST) {
			tessellateEndRodWest(p, tex);
		}
		else if (dir == Facing::EAST) {
			tessellateEndRodEast(p, tex);
		}
	}

	return true;
}

void BlockTessellator::tessellateEndRodUp(const BlockPos& p, const TextureUVCoordinateSet& tex) {
	static const float zero = 0.0f;
	static const float two = 2.0f / 16.0f;
	static const float four = 4.0f / 16.0f;
	static const float six = 6.0f / 16.0f;
	static const float seven = 7.0f / 16.0f;
	static const float fifteen = 15.0f / 16.0f;

	Vec3 first(6.0f / 16.0f, 0, 6.0f / 16.0f);
	Vec3 second(10.0f / 16.0f, 1.0f / 16.0f, 10.0f / 16.0f);
	Vec3 third(7.0f / 16.0f, 1.0f / 16.0f, 7.0f / 16.0f);
	Vec3 fourth(9.0f / 16.0f, 1.0f, 9.0f / 16.0f);

	Vec2 zerozero(two, two);
	Vec2 zeroone(six, two);
	Vec2 onezero(two, six);
	Vec2 oneone(six, six);

	mCurrentShape.set(first, second);     // Base

	Vec3 alpha = Vec3(p) + mCurrentShape.min;
	Vec3 beta = Vec3(p) + mCurrentShape.max;

	offsetUVs(tex, &zerozero, &zeroone, &onezero, &oneone);
	renderVerticesAndUVs(tex, alpha, beta, oneone, zeroone, zerozero, onezero, Facing::UP);
	renderVerticesAndUVs(tex, alpha, beta, onezero, zerozero, zeroone, oneone, Facing::DOWN);

	zerozero = { two, six };
	zeroone = { six, six };
	onezero = { two, seven };
	oneone = { six, seven };
	offsetUVs(tex, &zerozero, &zeroone, &onezero, &oneone);
	renderVerticesAndUVs(tex, alpha, beta, zeroone, zerozero, onezero, oneone, Facing::NORTH);
	renderVerticesAndUVs(tex, alpha, beta, zerozero, onezero, oneone, zeroone, Facing::SOUTH);
	renderVerticesAndUVs(tex, alpha, beta, zeroone, zerozero, onezero, oneone, Facing::WEST);
	renderVerticesAndUVs(tex, alpha, beta, onezero, oneone, zeroone, zerozero, Facing::EAST);

	mCurrentShape.set(third, fourth);     // Rod

	alpha = Vec3(p) + mCurrentShape.min;
	beta = Vec3(p) + mCurrentShape.max;

	zerozero = { two, zero };
	zeroone = { four, zero };
	onezero = { two, two };
	oneone = { four, two };
	offsetUVs(tex, &zerozero, &zeroone, &onezero, &oneone);
	renderVerticesAndUVs(tex, alpha, beta, oneone, zeroone, zerozero, onezero, Facing::UP);
	renderVerticesAndUVs(tex, alpha, beta, onezero, zerozero, zeroone, oneone, Facing::DOWN);

	zerozero = { zero, zero };
	zeroone = { two, zero };
	onezero = { zero, fifteen };
	oneone = { two, fifteen };
	offsetUVs(tex, &zerozero, &zeroone, &onezero, &oneone);
	renderVerticesAndUVs(tex, alpha, beta, zeroone, zerozero, onezero, oneone, Facing::NORTH);
	renderVerticesAndUVs(tex, alpha, beta, zerozero, onezero, oneone, zeroone, Facing::SOUTH);
	renderVerticesAndUVs(tex, alpha, beta, zeroone, zerozero, onezero, oneone, Facing::WEST);
	renderVerticesAndUVs(tex, alpha, beta, onezero, oneone, zeroone, zerozero, Facing::EAST);
}

void BlockTessellator::tessellateEndRodDown(const BlockPos& p, const TextureUVCoordinateSet& tex) {
	static const float zero = 0.0f;
	static const float two = 2.0f / 16.0f;
	static const float four = 4.0f / 16.0f;
	static const float six = 6.0f / 16.0f;
	static const float seven = 7.0f / 16.0f;
	static const float fifteen = 15.0f / 16.0f;

	Vec3 first(7.0f / 16.0f, 0, 7.0f / 16.0f);
	Vec3 second(9.0f / 16.0f, 15.0f / 16.0f, 9.0f / 16.0f);
	Vec3 third(6.0f / 16.0f, 15.0f / 16.0f, 6.0f / 16.0f);
	Vec3 fourth(10.0f / 16.0f, 1.0f, 10.0f / 16.0f);

	Vec2 zerozero(two, zero);
	Vec2 zeroone(four, zero);
	Vec2 onezero(two, two);
	Vec2 oneone(four, two);

	mCurrentShape.set(first, second);

	Vec3 alpha = Vec3(p) + mCurrentShape.min;
	Vec3 beta = Vec3(p) + mCurrentShape.max;

	offsetUVs(tex, &zerozero, &zeroone, &onezero, &oneone);
	renderVerticesAndUVs(tex, alpha, beta, oneone, zeroone, zerozero, onezero, Facing::UP);
	renderVerticesAndUVs(tex, alpha, beta, onezero, zerozero, zeroone, oneone, Facing::DOWN);

	zerozero = { two, fifteen };
	zeroone = { zero, fifteen };
	onezero = { two, zero };
	oneone = { zero, zero };
	offsetUVs(tex, &zerozero, &zeroone, &onezero, &oneone);
	renderVerticesAndUVs(tex, alpha, beta, zeroone, zerozero, onezero, oneone, Facing::NORTH);
	renderVerticesAndUVs(tex, alpha, beta, zerozero, onezero, oneone, zeroone, Facing::SOUTH);
	renderVerticesAndUVs(tex, alpha, beta, zeroone, zerozero, onezero, oneone, Facing::WEST);
	renderVerticesAndUVs(tex, alpha, beta, onezero, oneone, zeroone, zerozero, Facing::EAST);

	mCurrentShape.set(third, fourth);     // Base

	alpha = Vec3(p) + mCurrentShape.min;
	beta = Vec3(p) + mCurrentShape.max;

	zerozero = { six, six };
	zeroone = { two, six };
	onezero = { six, two };
	oneone = { two, two };
	offsetUVs(tex, &zerozero, &zeroone, &onezero, &oneone);
	renderVerticesAndUVs(tex, alpha, beta, oneone, zeroone, zerozero, onezero, Facing::UP);
	renderVerticesAndUVs(tex, alpha, beta, onezero, zerozero, zeroone, oneone, Facing::DOWN);

	zerozero = { two, six };
	zeroone = { six, six };
	onezero = { two, seven };
	oneone = { six, seven };
	offsetUVs(tex, &zerozero, &zeroone, &onezero, &oneone);
	renderVerticesAndUVs(tex, alpha, beta, zeroone, zerozero, onezero, oneone, Facing::NORTH);
	renderVerticesAndUVs(tex, alpha, beta, zerozero, onezero, oneone, zeroone, Facing::SOUTH);
	renderVerticesAndUVs(tex, alpha, beta, zeroone, zerozero, onezero, oneone, Facing::WEST);
	renderVerticesAndUVs(tex, alpha, beta, onezero, oneone, zeroone, zerozero, Facing::EAST);
}

void BlockTessellator::tessellateEndRodNorth(const BlockPos& p, const TextureUVCoordinateSet& tex) {
	static const float zero = 0.0f;
	static const float two = 2.0f / 16.0f;
	static const float four = 4.0f / 16.0f;
	static const float six = 6.0f / 16.0f;
	static const float seven = 7.0f / 16.0f;
	static const float fifteen = 15.0f / 16.0f;

	Vec3 first(7.0f / 16.0f, 7.0f / 16.0f, 1.0f / 16.0f);
	Vec3 second(9.0f / 16.0f, 9.0f / 16.0f, 1.0f);
	Vec3 third(6.0f / 16.0f, 6.0f / 16.0f, 0);
	Vec3 fourth(10.0f / 16.0f, 10.0f / 16.0f, 1.0f / 16.0f);

	Vec2 zerozero(zero, zero);
	Vec2 zeroone(two, zero);
	Vec2 onezero(zero, fifteen);
	Vec2 oneone(two, fifteen);

	mCurrentShape.set(first, second);

	Vec3 alpha = Vec3(p) + mCurrentShape.min;
	Vec3 beta = Vec3(p) + mCurrentShape.max;

	offsetUVs(tex, &zerozero, &zeroone, &onezero, &oneone);
	renderVerticesAndUVs(tex, alpha, beta, zerozero, onezero, oneone, zeroone, Facing::UP);
	renderVerticesAndUVs(tex, alpha, beta, zeroone, oneone, onezero, zerozero, Facing::DOWN);

	zerozero = { two, zero };
	zeroone = { four, zero };
	onezero = { two, two };
	oneone = { four, two };
	offsetUVs(tex, &zerozero, &zeroone, &onezero, &oneone);
	renderVerticesAndUVs(tex, alpha, beta, oneone, zeroone, zerozero, onezero, Facing::NORTH);
	renderVerticesAndUVs(tex, alpha, beta, onezero, zerozero, zeroone, oneone, Facing::SOUTH);

	zerozero = { zero, fifteen };
	zeroone = { zero, zero };
	onezero = { two, fifteen };
	oneone = { two, zero };
	offsetUVs(tex, &zerozero, &zeroone, &onezero, &oneone);
	renderVerticesAndUVs(tex, alpha, beta, zeroone, zerozero, onezero, oneone, Facing::WEST);
	renderVerticesAndUVs(tex, alpha, beta, oneone, onezero, zerozero, zeroone, Facing::EAST);

	mCurrentShape.set(third, fourth);     // Base

	alpha = Vec3(p) + mCurrentShape.min;
	beta = Vec3(p) + mCurrentShape.max;

	zerozero = { two, six };
	zeroone = { six, six };
	onezero = { two, seven };
	oneone = { six, seven };
	offsetUVs(tex, &zerozero, &zeroone, &onezero, &oneone);
	renderVerticesAndUVs(tex, alpha, beta, zerozero, onezero, oneone, zeroone, Facing::UP);
	renderVerticesAndUVs(tex, alpha, beta, zeroone, oneone, onezero, zerozero, Facing::DOWN);

	zerozero = { two, two };
	zeroone = { six, two };
	onezero = { two, six };
	oneone = { six, six };
	offsetUVs(tex, &zerozero, &zeroone, &onezero, &oneone);
	renderVerticesAndUVs(tex, alpha, beta, oneone, zeroone, zerozero, onezero, Facing::NORTH);
	renderVerticesAndUVs(tex, alpha, beta, onezero, zerozero, zeroone, oneone, Facing::SOUTH);

	zerozero = { two, seven };
	zeroone = { two, six };
	onezero = { six, seven };
	oneone = { six, six };
	offsetUVs(tex, &zerozero, &zeroone, &onezero, &oneone);
	renderVerticesAndUVs(tex, alpha, beta, zeroone, zerozero, onezero, oneone, Facing::WEST);
	renderVerticesAndUVs(tex, alpha, beta, oneone, onezero, zerozero, zeroone, Facing::EAST);
}

void BlockTessellator::tessellateEndRodSouth(const BlockPos& p, const TextureUVCoordinateSet& tex) {
	static const float zero = 0.0f;
	static const float two = 2.0f / 16.0f;
	static const float four = 4.0f / 16.0f;
	static const float six = 6.0f / 16.0f;
	static const float seven = 7.0f / 16.0f;
	static const float fifteen = 15.0f / 16.0f;

	Vec3 first(6.0f / 16.0f, 6.0f / 16.0f, 15.0f / 16.0f);
	Vec3 second(10.0f / 16.0f, 10.0f / 16.0f, 1.0f);
	Vec3 third(7.0f / 16.0f, 7.0f / 16.0f, 0);
	Vec3 fourth(9.0f / 16.0f, 9.0f / 16.0f, 15.0f / 16.0f);

	Vec2 zerozero(six, seven);
	Vec2 zeroone(two, seven);
	Vec2 onezero(six, six);
	Vec2 oneone(two, six);

	mCurrentShape.set(first, second);     // Base

	Vec3 alpha = Vec3(p) + mCurrentShape.min;
	Vec3 beta = Vec3(p) + mCurrentShape.max;

	
	offsetUVs(tex, &zerozero, &zeroone, &onezero, &oneone);
	renderVerticesAndUVs(tex, alpha, beta, zerozero, onezero, oneone, zeroone, Facing::UP);
	renderVerticesAndUVs(tex, alpha, beta, zeroone, oneone, onezero, zerozero, Facing::DOWN);

	zerozero = { six, six };
	zeroone = { two, six };
	onezero = { six, two };
	oneone = { two, two };
	offsetUVs(tex, &zerozero, &zeroone, &onezero, &oneone);
	renderVerticesAndUVs(tex, alpha, beta, oneone, zeroone, zerozero, onezero, Facing::NORTH);
	renderVerticesAndUVs(tex, alpha, beta, onezero, zerozero, zeroone, oneone, Facing::SOUTH);

	zerozero = { six, six };
	zeroone = { six, seven };
	onezero = { two, six };
	oneone = { two, seven };
	offsetUVs(tex, &zerozero, &zeroone, &onezero, &oneone);
	renderVerticesAndUVs(tex, alpha, beta, zeroone, zerozero, onezero, oneone, Facing::WEST);
	renderVerticesAndUVs(tex, alpha, beta, oneone, onezero, zerozero, zeroone, Facing::EAST);

	mCurrentShape.set(third, fourth);

	alpha = Vec3(p) + mCurrentShape.min;
	beta = Vec3(p) + mCurrentShape.max;

	zerozero = { two, fifteen };
	zeroone = { zero, fifteen };
	onezero = { two, zero };
	oneone = { zero, zero };
	offsetUVs(tex, &zerozero, &zeroone, &onezero, &oneone);
	renderVerticesAndUVs(tex, alpha, beta, zerozero, onezero, oneone, zeroone, Facing::UP);
	renderVerticesAndUVs(tex, alpha, beta, zeroone, oneone, onezero, zerozero, Facing::DOWN);

	zerozero = { four, two };
	zeroone = { two, two };
	onezero = { four, zero };
	oneone = { two, zero };
	offsetUVs(tex, &zerozero, &zeroone, &onezero, &oneone);
	renderVerticesAndUVs(tex, alpha, beta, oneone, zeroone, zerozero, onezero, Facing::NORTH);
	renderVerticesAndUVs(tex, alpha, beta, onezero, zerozero, zeroone, oneone, Facing::SOUTH);

	zerozero = { two, zero };
	zeroone = { two, fifteen };
	onezero = { zero, zero };
	oneone = { zero, fifteen };
	offsetUVs(tex, &zerozero, &zeroone, &onezero, &oneone);
	renderVerticesAndUVs(tex, alpha, beta, zeroone, zerozero, onezero, oneone, Facing::WEST);
	renderVerticesAndUVs(tex, alpha, beta, oneone, onezero, zerozero, zeroone, Facing::EAST);
}

void BlockTessellator::tessellateEndRodWest(const BlockPos& p, const TextureUVCoordinateSet& tex) {
	static const float zero = 0.0f;
	static const float two = 2.0f / 16.0f;
	static const float four = 4.0f / 16.0f;
	static const float six = 6.0f / 16.0f;
	static const float seven = 7.0f / 16.0f;
	static const float fifteen = 15.0f / 16.0f;

	Vec3 first(0, 6.0f / 16.0f, 6.0f / 16.0f);
	Vec3 second(1.0f / 16.0f, 10.0f / 16.0f, 10.0f / 16.0f);
	Vec3 third(1.0f / 16.0f, 7.0f / 16.0f, 7.0f / 16.0f);
	Vec3 fourth(1.0f, 9.0f / 16.0f, 9.0f / 16.0f);

	Vec2 zerozero(six, six);
	Vec2 zeroone(six, seven);
	Vec2 onezero(two, six);
	Vec2 oneone(two, seven);

	mCurrentShape.set(first, second);     // Base

	Vec3 alpha = Vec3(p) + mCurrentShape.min;
	Vec3 beta = Vec3(p) + mCurrentShape.max;

	
	offsetUVs(tex, &zerozero, &zeroone, &onezero, &oneone);
	renderVerticesAndUVs(tex, alpha, beta, oneone, onezero, zerozero, zeroone, Facing::UP);
	renderVerticesAndUVs(tex, alpha, beta, zeroone, zerozero, onezero, oneone, Facing::DOWN);

	zerozero = { two, two };
	zeroone = { six, two };
	onezero = { two, six };
	oneone = { six, six };
	offsetUVs(tex, &zerozero, &zeroone, &onezero, &oneone);
	renderVerticesAndUVs(tex, alpha, beta, onezero, zerozero, zeroone, oneone, Facing::WEST);
	renderVerticesAndUVs(tex, alpha, beta, oneone, zeroone, zerozero, onezero, Facing::EAST);

	zerozero = { six, six };
	zeroone = { six, seven };
	onezero = { two, six };
	oneone = { two, seven };
	offsetUVs(tex, &zerozero, &zeroone, &onezero, &oneone);
	renderVerticesAndUVs(tex, alpha, beta, zeroone, zerozero, onezero, oneone, Facing::NORTH);
	renderVerticesAndUVs(tex, alpha, beta, zerozero, onezero, oneone, zeroone, Facing::SOUTH);

	mCurrentShape.set(third, fourth);

	alpha = Vec3(p) + mCurrentShape.min;
	beta = Vec3(p) + mCurrentShape.max;

	zerozero = { zero, zero };
	zeroone = { two, zero };
	onezero = { zero, fifteen };
	oneone = { two, fifteen };
	offsetUVs(tex, &zerozero, &zeroone, &onezero, &oneone);
	renderVerticesAndUVs(tex, alpha, beta, oneone, onezero, zerozero, zeroone, Facing::UP);
	renderVerticesAndUVs(tex, alpha, beta, zeroone, zerozero, onezero, oneone, Facing::DOWN);

	zerozero = { two, zero };
	zeroone = { four, zero };
	onezero = { two, two };
	oneone = { four, two };
	offsetUVs(tex, &zerozero, &zeroone, &onezero, &oneone);
	renderVerticesAndUVs(tex, alpha, beta, onezero, zerozero, zeroone, oneone, Facing::EAST);
	renderVerticesAndUVs(tex, alpha, beta, oneone, zeroone, zerozero, onezero, Facing::WEST);

	zerozero = { two, zero };
	zeroone = { two, fifteen };
	onezero = { zero, zero };
	oneone = { zero, fifteen };
	offsetUVs(tex, &zerozero, &zeroone, &onezero, &oneone);
	renderVerticesAndUVs(tex, alpha, beta, zeroone, zerozero, onezero, oneone, Facing::NORTH);
	renderVerticesAndUVs(tex, alpha, beta, zerozero, onezero, oneone, zeroone, Facing::SOUTH);
}

void BlockTessellator::tessellateEndRodEast(const BlockPos& p, const TextureUVCoordinateSet& tex) {
	static const float zero = 0.0f;
	static const float two = 2.0f / 16.0f;
	static const float four = 4.0f / 16.0f;
	static const float six = 6.0f / 16.0f;
	static const float seven = 7.0f / 16.0f;
	static const float fifteen = 15.0f / 16.0f;

	Vec3 first(0, 7.0f / 16.0f, 7.0f / 16.0f);
	Vec3 second(15.0f / 16.0f, 9.0f / 16.0f, 9.0f / 16.0f);
	Vec3 third(15.0f / 16.0f, 6.0f / 16.0f, 6.0f / 16.0f);
	Vec3 fourth(1.0f, 10.0f / 16.0f, 10.0f / 16.0f);

	Vec2 zerozero(zero, zero);
	Vec2 zeroone(two, zero);
	Vec2 onezero(zero, fifteen);
	Vec2 oneone(two, fifteen);

	mCurrentShape.set(first, second);

	Vec3 alpha = Vec3(p) + mCurrentShape.min;
	Vec3 beta = Vec3(p) + mCurrentShape.max;

	
	offsetUVs(tex, &zerozero, &zeroone, &onezero, &oneone);
	renderVerticesAndUVs(tex, alpha, beta, zeroone, zerozero, onezero, oneone, Facing::UP);
	renderVerticesAndUVs(tex, alpha, beta, oneone, onezero, zerozero, zeroone, Facing::DOWN);

	zerozero = { two, zero };
	zeroone = { four, zero };
	onezero = { two, two };
	oneone = { four, two };
	offsetUVs(tex, &zerozero, &zeroone, &onezero, &oneone);
	renderVerticesAndUVs(tex, alpha, beta, oneone, zeroone, zerozero, onezero, Facing::EAST);
	renderVerticesAndUVs(tex, alpha, beta, onezero, zerozero, zeroone, oneone, Facing::WEST);

	zerozero = { zero, fifteen };
	zeroone = { zero, zero };
	onezero = { two, fifteen };
	oneone = { two, zero };
	offsetUVs(tex, &zerozero, &zeroone, &onezero, &oneone);
	renderVerticesAndUVs(tex, alpha, beta, zeroone, zerozero, onezero, oneone, Facing::NORTH);
	renderVerticesAndUVs(tex, alpha, beta, zerozero, onezero, oneone, zeroone, Facing::SOUTH);

	mCurrentShape.set(third, fourth);     // Base

	alpha = Vec3(p) + mCurrentShape.min;
	beta = Vec3(p) + mCurrentShape.max;

	zerozero = { six, six };
	zeroone = { six, seven };
	onezero = { two, six };
	oneone = { two, seven };
	offsetUVs(tex, &zerozero, &zeroone, &onezero, &oneone);
	renderVerticesAndUVs(tex, alpha, beta, zeroone, zerozero, onezero, oneone, Facing::UP);
	renderVerticesAndUVs(tex, alpha, beta, oneone, onezero, zerozero, zeroone, Facing::DOWN);

	zerozero = { two, two };
	zeroone = { six, two };
	onezero = { two, six };
	oneone = { six, six };
	offsetUVs(tex, &zerozero, &zeroone, &onezero, &oneone);
	renderVerticesAndUVs(tex, alpha, beta, oneone, zeroone, zerozero, onezero, Facing::WEST);
	renderVerticesAndUVs(tex, alpha, beta, onezero, zerozero, zeroone, oneone, Facing::EAST);

	zerozero = { two, seven };
	zeroone = { two, six };
	onezero = { six, seven };
	oneone = { six, six };
	offsetUVs(tex, &zerozero, &zeroone, &onezero, &oneone);
	renderVerticesAndUVs(tex, alpha, beta, zeroone, zerozero, onezero, oneone, Facing::NORTH);
	renderVerticesAndUVs(tex, alpha, beta, zerozero, onezero, oneone, zeroone, Facing::SOUTH);
}

void BlockTessellator::tessellateEndRodAppended(const Block& block, const BlockGraphics& blockGraphics) {
	auto& p = BlockPos::ZERO;
	auto& tex = blockGraphics.getTexture(Facing::DOWN);

	static const float zero = 0.0f;
	static const float two = 2.0f / 16.0f;
	static const float four = 4.0f / 16.0f;
	static const float six = 6.0f / 16.0f;
	static const float seven = 7.0f / 16.0f;
	static const float fifteen = 15.0f / 16.0f;

	//_tex1(toUV(getLightColor(p)));
	mTessellator.color(1.0f, 1.0f, 1.0f);

	Vec3 first(6.0f / 16.0f, 0, 6.0f / 16.0f);
	Vec3 second(10.0f / 16.0f, 1.0f / 16.0f, 10.0f / 16.0f);
	Vec3 third(7.0f / 16.0f, 1.0f / 16.0f, 7.0f / 16.0f);
	Vec3 fourth(9.0f / 16.0f, 1.0f, 9.0f / 16.0f);

	mCurrentShape.set(first, second);
	renderFaceWithUVs(block, p, tex, two, two, six, six, Facing::UP);
	renderFaceWithUVs(block, p, tex, six, six, two, two, Facing::DOWN);
	renderFaceWithUVs(block, p, tex, two, six, six, seven, Facing::NORTH);
	renderFaceWithUVs(block, p, tex, two, six, six, seven, Facing::SOUTH);
	renderFaceWithUVs(block, p, tex, two, six, six, seven, Facing::WEST);
	renderFaceWithUVs(block, p, tex, two, six, six, seven, Facing::EAST);

	mCurrentShape.set(third, fourth);
	renderFaceWithUVs(block, p, tex, two, zero, four, two, Facing::UP);
	renderFaceWithUVs(block, p, tex, four, two, two, zero, Facing::DOWN);
	renderFaceWithUVs(block, p, tex, zero, zero, two, fifteen, Facing::NORTH);
	renderFaceWithUVs(block, p, tex, zero, zero, two, fifteen, Facing::SOUTH);
	renderFaceWithUVs(block, p, tex, zero, zero, two, fifteen, Facing::WEST);
	renderFaceWithUVs(block, p, tex, zero, zero, two, fifteen, Facing::EAST);
}

void BlockTessellator::tessellateEndRodGui(const Block& block, const BlockGraphics& blockGraphics) {
	auto& p = BlockPos::ZERO;
	auto& tex = blockGraphics.getTexture(Facing::DOWN);

	static const float zero = 0.0f;
	static const float two = 2.0f / 16.0f;
	static const float four = 4.0f / 16.0f;
	static const float six = 6.0f / 16.0f;
	static const float seven = 7.0f / 16.0f;
	static const float fifteen = 15.0f / 16.0f;

	mTessellator.color(1.f, 1.f, 1.f);

	Vec3 first(6.0f / 16.0f, 0, 6.0f / 16.0f);
	Vec3 second(10.0f / 16.0f, 1.0f / 16.0f, 10.0f / 16.0f);
	Vec3 third(7.0f / 16.0f, 1.0f / 16.0f, 7.0f / 16.0f);
	Vec3 fourth(9.0f / 16.0f, 1.0f, 9.0f / 16.0f);

	mCurrentShape.set(first, second);
	renderFaceWithUVs(block, p, tex, two, two, six, six, Facing::UP);
	renderFaceWithUVs(block, p, tex, six, six, two, two, Facing::DOWN);
	renderFaceWithUVs(block, p, tex, two, six, six, seven, Facing::NORTH);
	renderFaceWithUVs(block, p, tex, two, six, six, seven, Facing::SOUTH);
	renderFaceWithUVs(block, p, tex, two, six, six, seven, Facing::WEST);
	renderFaceWithUVs(block, p, tex, two, six, six, seven, Facing::EAST);

	mCurrentShape.set(third, fourth);
	renderFaceWithUVs(block, p, tex, two, zero, four, two, Facing::UP);
	renderFaceWithUVs(block, p, tex, four, two, two, zero, Facing::DOWN);
	renderFaceWithUVs(block, p, tex, zero, zero, two, fifteen, Facing::NORTH);
	renderFaceWithUVs(block, p, tex, zero, zero, two, fifteen, Facing::SOUTH);
	renderFaceWithUVs(block, p, tex, zero, zero, two, fifteen, Facing::WEST);
	renderFaceWithUVs(block, p, tex, zero, zero, two, fifteen, Facing::EAST);
}

int BlockTessellator::blend(int a, int b, int c, int def) {
	return 0;
}

const bool BlockTessellator::isTranslucent(const Block& tt) {
	return false;
}

bool BlockTessellator::isSolidRender(const Block& tt) {
	return false;
}

float BlockTessellator::_occlusion(const BlockPos& pos) const {
	auto t = mRegion->getBlockID(pos);

	float trans = Block::mTranslucency[t];
	if(Block::mLightEmission[t]) {
		return 1.f;
	} else {
		auto br = 1.0f;
		return trans ? (trans * br) : br;
	}
}

int BlockTessellator::getLightEmission(const Block& block) {
	return Block::getLightEmission(block.mID);
}

BrightnessPair BlockTessellator::getLightColor(const BlockPos& belowPos) {
	if(mRenderingGUI) {
		return{ Brightness::MAX, Brightness::MAX };
	} else{
		return mBlockCache.getLightColor(belowPos);
	}
}

// Helper function to render all 6 directions of a block 
void BlockTessellator::_renderAllFaces(const Block& block, const BlockPos& pos, int data) {
	const BlockGraphics& blockGraphics = *BlockGraphics::mBlocks[block.getId()];
	renderFaceUp(block, pos, blockGraphics.getTexture(Facing::UP, data));
	renderFaceDown(block, pos, blockGraphics.getTexture(Facing::DOWN, data));
	renderNorth(block, pos, blockGraphics.getTexture(Facing::NORTH, data));
	renderSouth(block, pos, blockGraphics.getTexture(Facing::SOUTH, data));
	renderWest(block, pos, blockGraphics.getTexture(Facing::WEST, data));
	renderEast(block, pos, blockGraphics.getTexture(Facing::EAST, data));
}

static const int END_PORTAL_LAYER_COUNT = 16;

static const Vec2 END_PORTAL_UV_OFFSETS[END_PORTAL_LAYER_COUNT + 1] = {
	{ 0, 0 }, // Render solid black first
	{ 0.125f, 0.125f },
	{ 0.375f, 0.125f },
	{ 0.625f, 0.125f },
	{ 0.875f, 0.125f },
	{ 0.125f, 0.375f },
	{ 0.375f, 0.375f },
	{ 0.625f, 0.375f },
	{ 0.875f, 0.375f },
	{ 0.125f, 0.625f },
	{ 0.375f, 0.625f },
	{ 0.625f, 0.625f },
	{ 0.875f, 0.625f },
	{ 0.125f, 0.875f },
	{ 0.375f, 0.875f },
	{ 0.625f, 0.875f },
	{ 0.875f, 0.875f }
};

static const float END_PORTAL_PARALLAX_DEPTH = 32;

bool BlockTessellator::tessellateEndPortalInWorld(const Block& block, const BlockPos& pos, int data) {
	const float uOffsets[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	const float vOffsets[4] = { 0.0f, 1.0f, 1.0f, 0.0f };

	int layersToDraw = END_PORTAL_LAYER_COUNT;
	int layerStride = 1;
#ifdef MCPE_POCKET_EDITION
	layersToDraw /= 2;
	layerStride *= 2;
#endif
	float quantizedLayerDistance = END_PORTAL_PARALLAX_DEPTH / float(layersToDraw);
	for (int layer = 0; layer < layersToDraw + 1; ++layer) {
		float quantizedPlaneDepth = ((layersToDraw - layer) * quantizedLayerDistance) / END_PORTAL_PARALLAX_DEPTH;
		if (layer == 0) {
			quantizedPlaneDepth = 1;
		}
		const Vec3 surfaceNormal(0, 1, 0);
		const Vec3 quantizedNormal(surfaceNormal * 0.5f + Vec3(0.5f, 0.5f, 0.5f));
		for (int i = 0; i < 4; ++i) {
			// Give coordinates to the middle of each color on the 4x4 LUT
			mTessellator.tex(END_PORTAL_UV_OFFSETS[layer * layerStride]);
			// Encode <N * 0.5 + 0.5, (<portal face> - D)/64> of the parallax plane in vertex color
			mTessellator.color(Color(quantizedNormal.x, quantizedNormal.y, quantizedNormal.z, quantizedPlaneDepth));
			// Draw a happy plane
			mTessellator.vertex(pos.x + uOffsets[i], pos.y + .75f, pos.z + vOffsets[i]);
		}
	}

	return true;
}

bool BlockTessellator::tessellateEndGatewayInWorld(const Block& block, const BlockPos& pos, int data) {
	int layersToDraw = END_PORTAL_LAYER_COUNT;
	float quantizedLayerDistance = END_PORTAL_PARALLAX_DEPTH / float(layersToDraw);
	for (int layer = 0; layer < layersToDraw + 1; ++layer) {
		float quantizedPlaneDepth = ((layersToDraw - layer) * quantizedLayerDistance) / END_PORTAL_PARALLAX_DEPTH;
		if (layer == 0) {
			quantizedPlaneDepth = 1;
		}

#define QUANTIZE_NORMAL(n) ((n) * 0.5f + Vec3(0.5f, 0.5f, 0.5f));

		Vec3 quantizedNormal;
		// Give coordinates to the middle of each color on the 4x4 LUT
		mTessellator.tex(END_PORTAL_UV_OFFSETS[layer]);
		// Encode <N * 0.5 + 0.5, (<portal face> - D)/64> of the parallax plane in vertex color
		quantizedNormal = QUANTIZE_NORMAL(Vec3(0, 1, 0));
		mTessellator.color(Color(quantizedNormal.x, quantizedNormal.y, quantizedNormal.z, quantizedPlaneDepth));
		mTessellator.vertex(Vec3(pos) + Vec3::UNIT_Y);
		mTessellator.vertex(Vec3(pos) + Vec3::UNIT_Y + Vec3::UNIT_Z);
		mTessellator.vertex(Vec3(pos) + Vec3::UNIT_Y + Vec3::UNIT_Z + Vec3::UNIT_X);
		mTessellator.vertex(Vec3(pos) + Vec3::UNIT_Y + Vec3::UNIT_X);

		quantizedNormal = QUANTIZE_NORMAL(Vec3(0, -1, 0));
		mTessellator.color(Color(quantizedNormal.x, quantizedNormal.y, quantizedNormal.z, quantizedPlaneDepth));
		mTessellator.vertex(Vec3(pos));
		mTessellator.vertex(Vec3(pos) + Vec3::UNIT_X);
		mTessellator.vertex(Vec3(pos) + Vec3::UNIT_Z + Vec3::UNIT_X);
		mTessellator.vertex(Vec3(pos) + Vec3::UNIT_Z);

		quantizedNormal = QUANTIZE_NORMAL(Vec3(-1, 0, 0));
		mTessellator.color(Color(quantizedNormal.x, quantizedNormal.y, quantizedNormal.z, quantizedPlaneDepth));
		mTessellator.vertex(Vec3(pos));
		mTessellator.vertex(Vec3(pos) + Vec3::UNIT_Z);
		mTessellator.vertex(Vec3(pos) + Vec3::UNIT_Z + Vec3::UNIT_Y);
		mTessellator.vertex(Vec3(pos) + Vec3::UNIT_Y);

		quantizedNormal = QUANTIZE_NORMAL(Vec3(1, 0, 0));
		mTessellator.color(Color(quantizedNormal.x, quantizedNormal.y, quantizedNormal.z, quantizedPlaneDepth));
		mTessellator.vertex(Vec3(pos) + Vec3::UNIT_X);
		mTessellator.vertex(Vec3(pos) + Vec3::UNIT_X + Vec3::UNIT_Y);
		mTessellator.vertex(Vec3(pos) + Vec3::UNIT_X + Vec3::UNIT_Z + Vec3::UNIT_Y);
		mTessellator.vertex(Vec3(pos) + Vec3::UNIT_X + Vec3::UNIT_Z);

		quantizedNormal = QUANTIZE_NORMAL(Vec3(0, 0, -1));
		mTessellator.color(Color(quantizedNormal.x, quantizedNormal.y, quantizedNormal.z, quantizedPlaneDepth));
		mTessellator.vertex(Vec3(pos));
		mTessellator.vertex(Vec3(pos) + Vec3::UNIT_Y);
		mTessellator.vertex(Vec3(pos) + Vec3::UNIT_X + Vec3::UNIT_Y);
		mTessellator.vertex(Vec3(pos) + Vec3::UNIT_X);

		quantizedNormal = QUANTIZE_NORMAL(Vec3(0, 0, 1));
		mTessellator.color(Color(quantizedNormal.x, quantizedNormal.y, quantizedNormal.z, quantizedPlaneDepth));
		mTessellator.vertex(Vec3(pos) + Vec3::UNIT_Z);
		mTessellator.vertex(Vec3(pos) + Vec3::UNIT_Z + Vec3::UNIT_X);
		mTessellator.vertex(Vec3(pos) + Vec3::UNIT_Z + Vec3::UNIT_X + Vec3::UNIT_Y);
		mTessellator.vertex(Vec3(pos) + Vec3::UNIT_Z + Vec3::UNIT_Y);

#undef QUANTIZE_NORMAL
	}
	return true;
}

// Right now the only difference with each face is the use of a's & b's coordinates; this may be something that can be determined before passing vectors in
// Alternatively, this method could perhaps somehow transform UVs, but that seems much less likely as the UVs for each face will be different based on the
// Direction the object is facing.
void BlockTessellator::renderVerticesAndUVs(const TextureUVCoordinateSet& tex, const Vec3& a, const Vec3& b, Vec2& first, Vec2& second, Vec2& third, Vec2& fourth, const int face) {

	switch (face) {
	case Facing::UP:
		mTessellator.vertexUV(b.x, b.y, b.z, first.x, first.y);
		mTessellator.vertexUV(b.x, b.y, a.z, second.x, second.y);
		mTessellator.vertexUV(a.x, b.y, a.z, third.x, third.y);
		mTessellator.vertexUV(a.x, b.y, b.z, fourth.x, fourth.y);
		break;

	case Facing::DOWN:
		mTessellator.vertexUV(a.x, a.y, b.z, first.x, first.y);
		mTessellator.vertexUV(a.x, a.y, a.z, second.x, second.y);
		mTessellator.vertexUV(b.x, a.y, a.z, third.x, third.y);
		mTessellator.vertexUV(b.x, a.y, b.z, fourth.x, fourth.y);
		break;

	case Facing::NORTH:
		mTessellator.vertexUV(a.x, b.y, a.z, first.x, first.y);
		mTessellator.vertexUV(b.x, b.y, a.z, second.x, second.y);
		mTessellator.vertexUV(b.x, a.y, a.z, third.x, third.y);
		mTessellator.vertexUV(a.x, a.y, a.z, fourth.x, fourth.y);
		break;

	case Facing::SOUTH:
		mTessellator.vertexUV(a.x, b.y, b.z, first.x, first.y);
		mTessellator.vertexUV(a.x, a.y, b.z, second.x, second.y);
		mTessellator.vertexUV(b.x, a.y, b.z, third.x, third.y);
		mTessellator.vertexUV(b.x, b.y, b.z, fourth.x, fourth.y);
		break;

	case Facing::WEST:
		mTessellator.vertexUV(a.x, b.y, b.z, first.x, first.y);
		mTessellator.vertexUV(a.x, b.y, a.z, second.x, second.y);
		mTessellator.vertexUV(a.x, a.y, a.z, third.x, third.y);
		mTessellator.vertexUV(a.x, a.y, b.z, fourth.x, fourth.y);
		break;

	case Facing::EAST:
		mTessellator.vertexUV(b.x, a.y, b.z, first.x, first.y);
		mTessellator.vertexUV(b.x, a.y, a.z, second.x, second.y);
		mTessellator.vertexUV(b.x, b.y, a.z, third.x, third.y);
		mTessellator.vertexUV(b.x, b.y, b.z, fourth.x, fourth.y);
		break;

	default: break;
	}
}

void BlockTessellator::offsetUVs(const TextureUVCoordinateSet& tex, Vec2* first, Vec2* second, Vec2* third, Vec2* fourth) {
	first->x = tex.offsetWidth(first->x);
	first->y = tex.offsetHeight(first->y);

	if (second != nullptr) {
		second->x = tex.offsetWidth(second->x);
		second->y = tex.offsetHeight(second->y);
	}

	if (third != nullptr) {
		third->x = tex.offsetWidth(third->x);
		third->y = tex.offsetHeight(third->y);
	}

	if (fourth != nullptr) {
		fourth->x = tex.offsetWidth(fourth->x);
		fourth->y = tex.offsetHeight(fourth->y);
	}
}

void BlockTessellator::setUnrealActor(AActor* unrealActor)
{
	mUnrealActor = unrealActor;
}

void BlockTessellator::renderFaceWithUVs(const Block& block, const Vec3& p, const TextureUVCoordinateSet& tex, const float u0, const float v0, const float u1, const float v1, const int face) {
	TextureUVCoordinateSet newTex = tex;

	// Offset the UVs properly
	newTex._u0 = tex.offsetWidth(u0);
	newTex._v0 = tex.offsetHeight(v0);
	newTex._u1 = tex.offsetWidth(u1);
	newTex._v1 = tex.offsetHeight(v1);

	// Render the specified face
	switch (face) {
	case Facing::UP:
		renderFaceUp(block, p, newTex);
		break;

	case Facing::DOWN:
		renderFaceDown(block, p, newTex);
		break;

	case Facing::NORTH:
		renderNorth(block, p, newTex);
		break;

	case Facing::SOUTH:
		renderSouth(block, p, newTex);
		break;

	case Facing::WEST:
		renderWest(block, p, newTex);
		break;

	case Facing::EAST:
		renderEast(block, p, newTex);
		break;

	default:
		break;

	}

}

void BlockTessellator::_setShapeAndTessellate(const Vec3& min, const Vec3& max, const Block& block, const BlockPos& pos, const int data) {
	mCurrentShape.set(min, max);
	tessellateBlockInWorld(block, pos, data);
}

void BlockTessellator::clear()
{
	mTessellator.clear();
}
