/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "common_header.h"

// #include "Renderer/Mesh.h"
#include "world/phys/AABB.h"

#include "util/Bounds.h"
#include "client/renderer/renderer/TerrainLayer.h"
#include "world/level/block/Block.h"
#include "world/Facing.h"
#include "util/Boxed.h"

#include "client/renderer/chunks/VisibilityNode.h"
#include "client/renderer/chunks/RangeIndices.h"
// #include "Renderer/TextureGroup.h"
// #include "Renderer/TexturePtr.h"
#include "world/level/Tick.h"

class BlockSource;
class Entity;
class RenderChunkBuilder;
class RenderChunkSorter;
struct RenderChunkRenderParameters;

// @note: BlockEntity stuff is strippeddd away
class RenderChunk {
public:

	typedef std::array<unsigned char, RENDERCHUNK_SIDE * LEVEL_HEIGHT_DEPRECATED * RENDERCHUNK_SIDE> CutawayVisibilityBuffer;

	enum class BuildState {
		Dirty,
		Building,
		Empty,
		Ready
	};

	enum class SortState {
		SortDirty,
		Sorting,
		RestartSort,
		Ready
	};

	enum class DataState {
		WontLoad,
		Loading,
		Loaded
	};

// 	static void initMaterials(mce::TextureGroup& textures);

	typedef Boxed<RenderChunk> Ptr;

	bool mPartOfEmptyColumn = false;
	BlockPos mLastSortCameraDelta;
	float distanceFromPlayer2 = FLT_MAX;

	RenderChunk(const BlockPos& p);
	RenderChunk(const RenderChunk&) = delete;

	~RenderChunk();

	void setPos(const BlockPos& p);

	void startRebuild(Unique<RenderChunkBuilder> set);
	void startFaceSort(Unique<RenderChunkSorter> set);

	void rebuild(bool transparentLeaves, bool smoothLighting);
	void faceSort();

	Unique<RenderChunkBuilder>&& endRebuild(const Vec3& viewPos);
	Unique<RenderChunkSorter>&& endFaceSort();
	
	DataState getDataState(BlockSource& region);

	void setDirty(Tick now, bool immediateChange);
	bool isDirty(Tick now, uint32_t minTicksSinceLastChange = 0, uint32_t minTicksSinceFirstChange = 0) const;

	bool isFaceSortDirty() const;

	void setVisibilityChanged();
	void setVisible(bool v, unsigned int frameID);

	void updateDistanceFromPlayer(const Vec3& playerPos);

	int isVisited(unsigned int frameID) const;
	bool isVisible(int frameID) const;
	bool wasVisibileInPastFrames() const;

	bool isBottom() const;
	
	bool hasImmediateChange() const;

	bool isPending() const;
	bool isReady();

	bool hasLayer(TerrainLayer r) const;

	bool isCulled() const;
	bool isAllDark() const;

	float getAverageBrightness() const;

	bool canSeeChunkLookingFrom(unsigned char facingFrom, unsigned char facingTo) const;

	void reset();

	float distanceToSqr(const Entity* player) const;

	void render(const RenderChunkRenderParameters& renderChunkRenderParameters, const double currentTime);

	bool isEmpty() const;
	bool isSkyLit() const;

	int getPriority(const RenderChunk& r);
	BlockPos getFacing(int facing) const;
	const Vec3& getFaceCenter(int facing) const;

#ifdef DEBUG_STATS
	int getPolygonCount(TerrainLayer layer) const;
#endif

	bool isBuildState(BuildState s) const;
	bool isSortState(SortState s) const;

	static void setMaterialsBasedOnCameraUnderLiquid(const bool isUnderwater, const bool isUnderLava);

	BlockPos getPosition() const;
	void setPosition(BlockPos Position);

	BlockPos getCenter() const;
	void setCenter(BlockPos Center);

	Bounds getDataBounds() const;
	void setDataBounds(Bounds DataBounds);

	AABB getBoundingBox() const;
	void setBoundingBox(AABB BoundingBox);
	void updateFaceSortState(BlockPos cameraPos);
	void sortFaces();
	
	CutawayVisibilityBuffer& getCutawayVisibilityBuffer();

	const VisibilityNode& getVisibilityNode() const {
		return mVisibility;
	}

private:

	//TODO: this thing wastes a crapton of memory and it's even too big anyway!
	//Why is this LEVEL_HEIGHT high instead of 16x16x16???
	std::unique_ptr<CutawayVisibilityBuffer> mCutawayVisibilityBuffer;

	BlockPos mPosition = BlockPos(INT_MAX);
	BlockPos mCenter;

	Bounds mDataBounds;
	AABB mBoundingBox;

// 	mce::Mesh mMesh;

	Unique<RenderChunkBuilder> mCurrentBuilder;
	Unique<RenderChunkSorter> mCurrentSorter;

	bool mCanRender;
	std::atomic<BuildState> mBuildState;
	std::atomic<SortState> mSortState;
	bool mImmediateChange = false;

	VisibilityNode mVisibility;

	bool mVisibilityChanged = true;

	Vec3 mFaceCenters[6];
	bool mVisible = false;
	unsigned int mLastFrameVisited = 0;

	bool mSkyLit = false;
	float mAverageSkyLight = 0;
	bool mAllDark = false;

	double mReadyTime = 0;
	Tick mLastChangeTick, mFirstChangeTick;
	
	std::vector<uint16_t> mData;
	std::vector<uint32_t> mData32bit;

	uint32_t mIndexSize = 0;
	uint32_t mNumFaces = 0;
	RangeIndices mSortedRenderLayerIndexRange[(int)BlockRenderLayer::_RENDERLAYER_COUNT];

	std::vector<uint32_t> mFaces[Facing::NUM_CULLING_IDS];

	RangeIndices mIndexRange[(int)TerrainLayer::Count];
	RangeIndices mSortedIndexRange[(int)TerrainLayer::Count];
	RangeIndices mRenderLayerIndexRange[(int)BlockRenderLayer::_RENDERLAYER_COUNT];

	static mce::MaterialPtr sMaterialMap[enum_cast(TerrainLayer::Count)];
	static mce::MaterialPtr sFogMaterialMap[enum_cast(TerrainLayer::Count)];
	static mce::MaterialPtr sFadingChunksMaterial;
	static mce::MaterialPtr sFadingSeasonsChunksMaterial;
	static mce::MaterialPtr sFadingSeasonsAlphaChunkMaterial;
	static mce::MaterialPtr sBelowWaterStencilMaterial;
	static mce::MaterialPtr mAboveWaterBlendMaterial, mBelowWaterBlendMaterial, mAboveWaterBlendMaterialFog, mBelowWaterBlendMaterialFog;
// 	static mce::TexturePtr atlasTexture, foliageTexture, brightnessTexture, endPortalTexture, endPortalColorTexture;

#ifdef DEBUG_STATS
	int mPolyCount[enum_cast(TerrainLayer::Count)];
#endif

	bool _tryChangeBuildState(BuildState old, BuildState current);
	void _changeBuildState(BuildState old, BuildState current);
	bool _tryChangeSortState(SortState old, SortState current);
	void _changeSortState(SortState old, SortState current);

// 	const mce::MaterialPtr& _chooseMaterial( TerrainLayer layer, double currentTime, bool forceFog );

	static void _initLayers(mce::MaterialPtr* materials, const std::string& postfix);
public:
	void makeReadyAsEmpty(bool hasNoSkyLight);
};
