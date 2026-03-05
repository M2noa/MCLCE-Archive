#include "Dungeons.h"

#include "AmbientOcclusionCalculator.h"
#include "world/level/block/Block.h"
#include "world/level/BlockSource.h"
#include "world/level/material/Material.h"
#include "client/renderer/block/BlockTessellatorCache.h"
#include "client/renderer/block/BlockGraphics.h"
#include "world/level/dimension/Dimension.h"

AmbientOcclusionCalculator::AmbientOcclusionCalculator(const BlockPos& centerPos, const Block& centerBlock, BlockTessellatorCache& cache) :
	mCenterPos(centerPos)
	, mCenterBlock(centerBlock)
	, mCache(cache) {

	if (mCache.getRegion()->getDimensionConst().hasUpsideDownLight()) {
		mFaceShading[Facing::UP] = 0.7f;
		mFaceShading[Facing::DOWN] = 0.95f;
	}
	else {
		mFaceShading[Facing::UP] = 1.0f;
		mFaceShading[Facing::DOWN] = 0.5f;
	}
	mFaceShading[Facing::NORTH] = mFaceShading[Facing::SOUTH] = 0.8f;
	mFaceShading[Facing::WEST] = mFaceShading[Facing::EAST] = 0.6f;
}

void AmbientOcclusionCalculator::setBaseColor(const Color& color) {
	mBaseColor = color;
}

void AmbientOcclusionCalculator::setOutputColors(Color* colors) {
	mOutputColors = colors;
}

void AmbientOcclusionCalculator::setOutputLightTexturePositions(BrightnessPair* lightTexturePositions) {
	mOutputLightTexturePositons = lightTexturePositions;
}

void AmbientOcclusionCalculator::setTintSides(bool tint) {
	mTintSides = tint;
}

void AmbientOcclusionCalculator::setSeasons(bool seasons) {
	mDoSeasons = seasons;
}

void AmbientOcclusionCalculator::setTouchEdge(bool touch) {
	mTouchEdge = touch;
}

Color AmbientOcclusionCalculator::_computeAOColor(float baseAO, bool translucent, float gamma) const {
	auto ao = std::pow(baseAO, gamma);
	if (!mDoSeasons) {
		if (mTintSides) {
			return mBaseColor * ao;
		}

		return Color::fromIntensity(ao, translucent ? 1.f : ao);
	}
	return{
		mBaseColor.r,
		mBaseColor.g,
		(mTintSides) ? 1.0f : 0.0f,
		ao
	};
}

void AmbientOcclusionCalculator::calculate(FacingID face, bool translucent) {

	float ll[4];
	BrightnessPair ccx00;
	if (!mTouchEdge || !_isSolidRender(mCenterPos)) {
		ccx00 = mCache.getLightColor(mCenterPos);
	}
	else {
		ccx00 = mCache.getLightColor(mCenterPos.neighbor(face));
	}
	const float faceLight = _getShadeBrightness(mCenterPos.neighbor(face));

	BrightnessPair color[4] = { BrightnessPair{},BrightnessPair{},BrightnessPair{},BrightnessPair{} };
	BrightnessPair cornerColor[4] = { BrightnessPair{},BrightnessPair{},BrightnessPair{},BrightnessPair{} };
	BlockPos lightColorPositions[4], cornerPositions[4];

	float sideBrightness[4] = { 0.f, 0.f, 0.f, 0.f };

	auto pos = mTouchEdge ? mCenterPos.neighbor(face) : mCenterPos;
	switch (face) {
	case Facing::DOWN:
		lightColorPositions[0] = pos.north();
		lightColorPositions[1] = pos.west();
		lightColorPositions[2] = pos.south();
		lightColorPositions[3] = pos.east();

		cornerPositions[0] = pos.west().north();
		cornerPositions[1] = pos.west().south();
		cornerPositions[2] = pos.east().south();
		cornerPositions[3] = pos.east().north();
		break;
	case Facing::UP:
		lightColorPositions[0] = pos.north();
		lightColorPositions[1] = pos.east();
		lightColorPositions[2] = pos.south();
		lightColorPositions[3] = pos.west();

		cornerPositions[0] = pos.north().east();
		cornerPositions[1] = pos.south().east();
		cornerPositions[2] = pos.south().west();
		cornerPositions[3] = pos.north().west();
		break;
	case Facing::NORTH:
		lightColorPositions[0] = pos.above();
		lightColorPositions[1] = pos.west();
		lightColorPositions[2] = pos.below();
		lightColorPositions[3] = pos.east();

		cornerPositions[0] = pos.west().above();
		cornerPositions[1] = pos.west().below();
		cornerPositions[2] = pos.east().below();
		cornerPositions[3] = pos.east().above();
		break;
	case Facing::SOUTH:
		lightColorPositions[0] = pos.above();
		lightColorPositions[1] = pos.east();
		lightColorPositions[2] = pos.below();
		lightColorPositions[3] = pos.west();

		cornerPositions[0] = pos.east().above();
		cornerPositions[1] = pos.east().below();
		cornerPositions[2] = pos.west().below();
		cornerPositions[3] = pos.west().above();

		break;
	case Facing::WEST:

		lightColorPositions[0] = pos.above();
		lightColorPositions[1] = pos.south();
		lightColorPositions[2] = pos.below();
		lightColorPositions[3] = pos.north();

		cornerPositions[0] = pos.above().south();
		cornerPositions[1] = pos.below().south();
		cornerPositions[2] = pos.below().north();
		cornerPositions[3] = pos.above().north();
		break;
	case Facing::EAST:
		lightColorPositions[0] = pos.above();
		lightColorPositions[1] = pos.north();
		lightColorPositions[2] = pos.below();
		lightColorPositions[3] = pos.south();

		cornerPositions[0] = pos.above().north();
		cornerPositions[1] = pos.below().north();
		cornerPositions[2] = pos.below().south();
		cornerPositions[3] = pos.above().south();
		break;
	}

	for (auto a : range(4)) {
		color[a] = mCache.getLightColor(lightColorPositions[a]);
		sideBrightness[a] = _getShadeBrightness(lightColorPositions[a]);
	}

	// Will be set below
	float cornerLight[4];

	if (_notOccludedBy(lightColorPositions[0], lightColorPositions[1])) {
		cornerLight[0] = _getShadeBrightness(cornerPositions[0]);
		cornerColor[0] = mCache.getLightColor(cornerPositions[0]);
	}
	else {
		cornerLight[0] = sideBrightness[1];
		cornerColor[0] = color[1];
	}

	if (_notOccludedBy(lightColorPositions[1], lightColorPositions[2])) {
		cornerLight[1] = _getShadeBrightness(cornerPositions[1]);
		cornerColor[1] = mCache.getLightColor(cornerPositions[1]);
	}
	else {
		cornerLight[1] = sideBrightness[2];
		cornerColor[1] = color[2];
	}

	if (_notOccludedBy(lightColorPositions[2], lightColorPositions[3])) {
		cornerLight[2] = _getShadeBrightness(cornerPositions[2]);
		cornerColor[2] = mCache.getLightColor(cornerPositions[2]);
	}
	else {
		cornerLight[2] = sideBrightness[3];
		cornerColor[2] = color[3];
	}

	if (_notOccludedBy(lightColorPositions[3], lightColorPositions[0])) {
		cornerLight[3] = _getShadeBrightness(cornerPositions[3]);
		cornerColor[3] = mCache.getLightColor(cornerPositions[3]);
	}
	else {
		cornerLight[3] = sideBrightness[0];
		cornerColor[3] = color[0];
	}

	ll[0] = (faceLight + sideBrightness[0] + sideBrightness[1] + cornerLight[0]) / 4.0f;
	ll[1] = (faceLight + sideBrightness[1] + sideBrightness[2] + cornerLight[1]) / 4.0f;
	ll[2] = (faceLight + sideBrightness[2] + sideBrightness[3] + cornerLight[2]) / 4.0f;
	ll[3] = (faceLight + sideBrightness[3] + sideBrightness[0] + cornerLight[3]) / 4.0f;

	mOutputLightTexturePositons[0] = _blend(color[0], cornerColor[0], color[1], ccx00);
	mOutputLightTexturePositons[1] = _blend(color[1], cornerColor[1], color[2], ccx00);
	mOutputLightTexturePositons[2] = _blend(color[2], cornerColor[2], color[3], ccx00);
	mOutputLightTexturePositons[3] = _blend(color[3], cornerColor[3], color[0], ccx00);

	float gamma = mCenterBlock.getBrightnessGamma();
	auto& topBlock = mCache.getBlock(mCenterPos.above());
	auto& topBlockGraphics = *BlockGraphics::mBlocks[topBlock.getId()];
	float topGamma = topBlockGraphics.getBlockShape() != BlockShape::INVISIBLE ? topBlock.getBrightnessGamma() : gamma;

	auto topOutputColors = mOutputColors + 4;
	for (auto a : range(4)) {
		float ao = getShadingForFace(face) * ll[a];

		mOutputColors[a] = _computeAOColor(ao, translucent, gamma);
		topOutputColors[a] = (topGamma != gamma) ? _computeAOColor(ao, translucent, topGamma) : mOutputColors[a];
	}
}

float AmbientOcclusionCalculator::_getShadeBrightness(BlockPos const& pos) {
	const Block& block = mCache.getBlock(pos);//
	return block.getShadeBrightness();
}

bool AmbientOcclusionCalculator::isTranslucent(const BlockPos& pos) {
	const Block& block = mCache.getBlock(pos);
	return block.getMaterial().getTranslucency() != 0;
}

BrightnessPair AmbientOcclusionCalculator::_blend(BrightnessPair a, BrightnessPair b, BrightnessPair c, BrightnessPair def) {
	if (a.sky == 0 && a.block == 0) {
		a = def;
	}

	if (b.sky == 0 && b.block == 0) {
		b = def;
	}

	if (c.sky == 0 && c.block == 0) {
		c = def;
	}

	return{
		Brightness((((int)a.sky + b.sky + c.sky + def.sky) >> 2) & 0xff),
		Brightness((((int)a.block + b.block + c.block + def.block) >> 2) & 0xff),
	};
}

bool AmbientOcclusionCalculator::_isSolidRender(const BlockPos& pos) {
	const Block& block = mCache.getBlock(pos);
	return block.isSolid();
}

float AmbientOcclusionCalculator::getShadingForFace(FacingID face) {
	return mFaceShading[face];
}

bool AmbientOcclusionCalculator::_notOccludedBy(const BlockPos& neighbour1, const BlockPos& neighbour2) {
	return !_isSolidRender(neighbour1) || !_isSolidRender(neighbour2);
}
