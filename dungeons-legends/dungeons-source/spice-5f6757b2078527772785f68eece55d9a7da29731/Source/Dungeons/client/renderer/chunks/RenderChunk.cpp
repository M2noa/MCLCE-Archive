/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "client/renderer/chunks/RenderChunk.h"
#include "client/renderer/renderer/Tessellator.h"
// #include "client/renderer/renderer/RenderMaterialGroup.h"
#include "client/renderer/block/BlockTessellator.h"
#include "client/renderer/chunks/RenderChunkBuilder.h"
// #include "client/renderer/game/LevelRenderer.h"
// #include "client/renderer/game/RenderChunkRenderParameters.h"
#include "world/entity/Entity.h"
#include "world/level/BlockSource.h"
#include "world/level/chunk/ChunkViewSource.h"
#include "world/level/chunk/LevelChunk.h"
#include "util/DebugStats.h"
// #include "util/PerfTimer.h"
#include "util/Math.h"
#include "Core/Debug/Log.h"
// #include "Renderer/MaterialPtr.h"

// mce::MaterialPtr RenderChunk::sFadingChunksMaterial;
// mce::MaterialPtr RenderChunk::sFadingSeasonsChunksMaterial;
// mce::MaterialPtr RenderChunk::sFadingSeasonsAlphaChunkMaterial;
// mce::MaterialPtr RenderChunk::sBelowWaterStencilMaterial;
// mce::MaterialPtr RenderChunk::sMaterialMap[enum_cast(TerrainLayer::Count)] = { };
// mce::MaterialPtr RenderChunk::sFogMaterialMap[enum_cast(TerrainLayer::Count)] = { };
// mce::TexturePtr RenderChunk::endPortalTexture;
// mce::TexturePtr RenderChunk::endPortalColorTexture;
// 
// mce::TexturePtr RenderChunk::atlasTexture;
// mce::TexturePtr RenderChunk::foliageTexture;
// mce::TexturePtr RenderChunk::brightnessTexture;

void RenderChunk::_initLayers(mce::MaterialPtr* materials, const std::string& postfix) {
// 	materials[enum_cast(TerrainLayer::Opaque)] = mce::RenderMaterialGroup::switchable.getMaterial("terrain_opaque" + postfix);
// 	materials[enum_cast(TerrainLayer::OpaqueSeasons)] = mce::RenderMaterialGroup::switchable.getMaterial("terrain_opaque_seasons" + postfix);
// 	materials[enum_cast(TerrainLayer::Blend)] = mce::RenderMaterialGroup::switchable.getMaterial("terrain_blend" + postfix);
// 	materials[enum_cast(TerrainLayer::Water)] = mce::RenderMaterialGroup::switchable.getMaterial("terrain_water" + postfix);
// 	materials[enum_cast(TerrainLayer::Alpha)] = mce::RenderMaterialGroup::switchable.getMaterial("terrain_alpha" + postfix);
// 	materials[enum_cast(TerrainLayer::AlphaSingleSide)] = mce::RenderMaterialGroup::switchable.getMaterial("terrain_alpha_single_side" + postfix);
// 	materials[enum_cast(TerrainLayer::AlphaSeasons)] = mce::RenderMaterialGroup::switchable.getMaterial("terrain_alpha_seasons" + postfix);
// 	materials[enum_cast(TerrainLayer::DoubleSide)] = mce::RenderMaterialGroup::switchable.getMaterial("terrain_doubleside" + postfix);
// 	materials[enum_cast(TerrainLayer::Far)] = mce::RenderMaterialGroup::switchable.getMaterial("terrain_far" + postfix);
// 	materials[enum_cast(TerrainLayer::FarSeasons)] = mce::RenderMaterialGroup::switchable.getMaterial("terrain_seasons_far" + postfix);
// 	materials[enum_cast(TerrainLayer::FarSeasonsAlpha)] = mce::RenderMaterialGroup::switchable.getMaterial("terrain_seasons_far_alpha" + postfix);
// 	materials[enum_cast(TerrainLayer::EndPortal)] = mce::RenderMaterialGroup::switchable.getMaterial("portal_base" + postfix);
}

// void RenderChunk::initMaterials(mce::TextureGroup& textures) {
// 	sFadingChunksMaterial = mce::RenderMaterialGroup::switchable.getMaterial("terrain_fading_in");
// 	sFadingSeasonsChunksMaterial = mce::RenderMaterialGroup::switchable.getMaterial("terrain_seasons_fading_in");
// 	sFadingSeasonsAlphaChunkMaterial = mce::RenderMaterialGroup::switchable.getMaterial("terrain_seasons_fading_in_alpha");
// 	sBelowWaterStencilMaterial = mce::RenderMaterialGroup::switchable.getMaterial("terrain_water_below_stencil");
// 
// 	atlasTexture = textures.getTexture(ResourceLocation("atlas.terrain"));
// 	foliageTexture = textures.getTexture(DynamicTexture::SeasonFoliageTexture);
// 	brightnessTexture = textures.getTexture(DynamicTexture::BrightnessTexture);
// 	endPortalTexture = textures.getTexture(ResourceLocation("textures/entity/end_portal"));
// 	endPortalColorTexture = textures.getTexture(ResourceLocation("textures/environment/end_portal_colors"));
// 
// 	_initLayers(sMaterialMap, Util::EMPTY_STRING);
// 	_initLayers(sFogMaterialMap, "_fog");
// }

RenderChunk::RenderChunk(const BlockPos& p) :
	mBuildState(BuildState::Dirty),
	mSortState(SortState::SortDirty) {
	setPos(p);
}

RenderChunk::~RenderChunk() {
}

void RenderChunk::setPos(const BlockPos& p) {
	if (p == mPosition) {
		return;
	}

	reset();
	mPosition = p;
	mCenter = p + BlockPos(RENDERCHUNK_SIDE / 2);

	mBoundingBox = AABB(mPosition, (float)RENDERCHUNK_SIDE);

	//compute face centers
	const static int HALF_SIZE = RENDERCHUNK_SIDE / 2;

	for (auto i : range(6)) {
		BlockPos faceCenterBlock = mCenter + (Facing::DIRECTION[i] * HALF_SIZE);
		mFaceCenters[i] = Vec3((float)faceCenterBlock.x, (float)faceCenterBlock.y, (float)faceCenterBlock.z);
	}

	//compute bounds

	int r = 1;
	BlockPos min = p - r;
	BlockPos max = p + (RENDERCHUNK_SIDE + r);
	min.y = max.y = 0;

	mDataBounds = Bounds(min, max, RENDERCHUNK_SIDE);
}

RenderChunk::DataState RenderChunk::getDataState(BlockSource& region) {
	//check if all the chunks are existing & loaded
	for (auto& c : mDataBounds) {
		auto lc = region.getChunkSource().getExistingChunk(ChunkPos(c.x, c.z));

		if (lc == nullptr) {//not going to exist
			return DataState::WontLoad;
		}
		else if (lc->getState() < ChunkState::Loaded) {
			return DataState::Loading;
		}
	}
	return DataState::Loaded;
}

void RenderChunk::startRebuild(Unique<RenderChunkBuilder> set) {
	DEBUG_ASSERT(set, "Invalid set");
	DEBUG_ASSERT(isBuildState(BuildState::Dirty), "Render chunk is in an invalid state");

	mCurrentBuilder = std::move(set);

	//move the set region to our bounds
	mCurrentBuilder->localSource->move(mPosition - 1, mPosition + RENDERCHUNK_SIDE + 1);

	mCurrentBuilder->visibilityChanged = mVisibilityChanged;	// double buffer this
	mVisibilityChanged = false;
	mCurrentBuilder->mVisibility = mVisibility;

	mImmediateChange = false;

	//reset the dirty tick so that changes happening during rebuild are registered
	mLastChangeTick = mFirstChangeTick = Tick::MAX; 

	// change state and assign to a worker thread
	_changeBuildState(BuildState::Dirty, BuildState::Building);

	// Sorting state is now dirty
	_tryChangeSortState(SortState::Ready, SortState::SortDirty);

	// If the render chunk is already being sorted, restart the sort
	_tryChangeSortState(SortState::Sorting, SortState::RestartSort);

	// if sort state is already SortDirty or RestartSort, there's no need to change it.
}

void RenderChunk::startFaceSort(Unique<RenderChunkSorter> set) {
	DEBUG_ASSERT(set, "Invalid set");
	DEBUG_ASSERT(isSortState(SortState::SortDirty), "Render chunk is in an invalid state");

	mCurrentSorter = std::move(set);

	//change state and assign to a worker thread
	_changeSortState(SortState::SortDirty, SortState::Sorting);
}

void RenderChunk::faceSort() {

	if (mCurrentSorter == nullptr) {
		LOGI("Renderchunk is attempting to sort, but 'currentBuilder' is empty!!!\n");
		return;
	}

	mCurrentSorter->sortFaces(*this);
}

Unique<RenderChunkSorter>&& RenderChunk::endFaceSort() {
	DEBUG_ASSERT_MAIN_THREAD;

	DEBUG_ASSERT(mSortState == SortState::Sorting || mSortState == SortState::RestartSort, "Invalid state for ending sorting");

// 	ScopedProfile("End face sort");

	auto& builder = *mCurrentSorter;

	if (mSortState == SortState::Sorting) {
		// Update the index buffer for the sorted indices
		if (mIndexSize == sizeof(uint16_t)) {
			if (mData.size() > 0) {
// 				mMesh.loadIndexBuffer(mIndexSize, mData.data(), mData.size());
			}
		}
		else {
			if (mData32bit.size() > 0) {
// 				mMesh.loadIndexBuffer(mIndexSize, mData32bit.data(), mData32bit.size());
			}
		}

		// Set up the terrain layer ranges from the render layer ranges
		builder.setupIndexRanges(mSortedIndexRange, mSortedRenderLayerIndexRange);

		if (mSortState == SortState::Sorting) {
			_changeSortState(SortState::Sorting, SortState::Ready);
		}
	}
	else if (mSortState == SortState::RestartSort) {
		_changeSortState(SortState::RestartSort, SortState::SortDirty);
	}

	return std::move(mCurrentSorter);
}

void RenderChunk::rebuild(bool transparentLeaves, bool smoothLighting) {

	DEBUG_ASSERT(mCurrentBuilder, "Renderchunk is attempting to rebuild, but 'currentBuilder' is empty!!!\n");
	if (mCurrentBuilder == nullptr) {
		LOGI("Renderchunk is attempting to rebuild, but 'currentBuilder' is empty!!!\n");
		return;
	}

	mCurrentBuilder->build(*this, transparentLeaves, smoothLighting);
}

Unique<RenderChunkBuilder>&& RenderChunk::endRebuild(const Vec3& viewPos) {
	DEBUG_ASSERT_MAIN_THREAD;

	DEBUG_ASSERT(mBuildState == BuildState::Building, "Invalid state for ending rebuild");

// 	ScopedProfile("EndRebuild");

	//end the thing
	auto& builder = *mCurrentBuilder;
	auto& t = builder.renderer->getTessellator();

	builder.localSource->clear();

	if (mBuildState == BuildState::Building) {
		if (t.isTessellating()) {
// 			ScopedProfile("Upload RenderChunk");

// 			mMesh = t.END();
			const std::vector<FacingID>& quadFacingList = t.getFaces();

			mNumFaces = quadFacingList.size();

			// Initialize the quad facing lists from the tessellator quad list
			for (int i = 0; i < Facing::NUM_CULLING_IDS; i++) {
				mFaces[i].clear();
				mFaces[i].reserve(mNumFaces / Facing::NUM_CULLING_IDS);	// estimated reserve
			}

			// Place the quads in the correct collection
			for (uint32_t i = 0; i < quadFacingList.size(); i++) {
				mFaces[quadFacingList[i]].push_back(i);
			}

			t.setOffset(0, 0, 0);

			{
// 				ScopedProfile("Memory clean up");
				//clear up memory
				builder.trim();
			}

			//update polycounts
#ifdef DEBUG_STATS

			for (int i = 0; i < enum_cast(TerrainLayer::Count); ++i) {
				mPolyCount[i] = builder.mTerrainLayerRange[i].getPolyCount();
			}
#endif
		}
		else {
// 			mMesh.reset();

#ifdef DEBUG_STATS
			memset(mPolyCount, 0, sizeof(mPolyCount));
#endif
		}

		//copy the new state
		mVisibility = builder.mVisibility;
		mAverageSkyLight = builder.averageSkyLight;
		mAllDark = builder.allDark;
		mSkyLit = builder.skyLit;
		memcpy(mIndexRange, builder.mTerrainLayerRange, sizeof(mIndexRange));
		memcpy(mRenderLayerIndexRange, builder.mRenderLayerRange, sizeof(mRenderLayerIndexRange));

		if (mLastChangeTick < Tick::MAX) {
			//if the dirty tick was set while rebuilding, rebuild again... there might be some changes we didn't account for
			_changeBuildState(BuildState::Building, BuildState::Dirty);
		}
		else {
			_changeBuildState(BuildState::Building, isEmpty() ? BuildState::Empty : BuildState::Ready);
		}

		updateFaceSortState(viewPos);

		if (!mCanRender) {
			//store when the chunk was first ready to do the fade in
// 			mReadyTime = getTimeS();
		}

		mCanRender = true;

#ifdef DEBUG_STATS
		DebugStats::instance.mRebuiltChunks.push_back(ChunkPos(mPosition));
#endif
	}

	DEBUG_ASSERT(mCurrentBuilder, "Invalid set");
	return std::move(mCurrentBuilder);
}

void RenderChunk::makeReadyAsEmpty(bool hasNoSkyLight) {

	mVisibilityChanged = false;
	mImmediateChange = false;

// 	mMesh.reset();
	mNumFaces = 0;

	mVisibility = VisibilityNode(true);

	mAverageSkyLight = hasNoSkyLight ? Brightness::MIN : Brightness::MAX;
	mSkyLit = hasNoSkyLight;
	mAllDark = !hasNoSkyLight;

	mLastChangeTick = mFirstChangeTick = Tick::MAX;

	mCanRender = true;

#ifdef DEBUG_STATS
	memset(mPolyCount, 0, sizeof(mPolyCount));
	DebugStats::instance.mRebuiltChunks.push_back(ChunkPos(mPosition));
#endif

	_changeBuildState(BuildState::Dirty, BuildState::Ready);

	_tryChangeSortState(SortState::SortDirty, SortState::Ready);
}


// const mce::MaterialPtr& RenderChunk::_chooseMaterial(TerrainLayer layer, double currentTime, bool forceFog) {
// 	if (layer == TerrainLayer::Far || layer == TerrainLayer::FarSeasons || layer == TerrainLayer::FarSeasonsAlpha) {
// 		const float fadeInTime = 1.2f;
// 		double timediff = currentTime - mReadyTime;
// 		if (timediff < fadeInTime) {
// 			Color color = currentShaderColor.getColor();
// 			color.r = (float)((fadeInTime - timediff) / 5.);
// 			currentShaderColor.setColor(color);
// 
// 			switch (layer) {
// 				case TerrainLayer::Far:
// 					return sFadingChunksMaterial;
// 				case TerrainLayer::FarSeasons:
// 					return sFadingSeasonsChunksMaterial;
// 				case TerrainLayer::FarSeasonsAlpha:
// 					return sFadingSeasonsAlphaChunkMaterial;
// 				default:
// 					break;
// 			}
// 		}
// 	}
// 	else if (layer == TerrainLayer::WaterStencil) {
// 		return sBelowWaterStencilMaterial;
// 	}
// 
// 	auto& materialSet = forceFog ? sFogMaterialMap : sMaterialMap;
// 	return materialSet[(int)layer];
// }

void RenderChunk::updateFaceSortState(BlockPos cameraPos) {
	if (!(mSortState == SortState::Ready || mSortState == SortState::Sorting || mSortState == SortState::RestartSort || mSortState == SortState::SortDirty)) {
		return;
	}

	// Determine chunk position relative to the camera
	BlockPos chunkPos = mPosition / RENDERCHUNK_SIDE;
	BlockPos camPos = BlockPos((float)cameraPos.x / RENDERCHUNK_SIDE, (float)cameraPos.y / RENDERCHUNK_SIDE, (float)cameraPos.z / RENDERCHUNK_SIDE);
	BlockPos cameraDelta = chunkPos - camPos;

	// Clamp the camera delta.  
	// -1 signifies that the chunk is entirely in the negative space of the camera for that axis
	// 0 signifies that the camera is within the min/max bounds of the chunk for that axis
	// 1 signifies that the chunk is entirely in the positive space of the camera for that axis
	cameraDelta.x = Math::clamp(cameraDelta.x, -1, 1);
	cameraDelta.y = Math::clamp(cameraDelta.y, -1, 1);
	cameraDelta.z = Math::clamp(cameraDelta.z, -1, 1);

	// Sort the chunk indices if the camera delta has changed
	if (mLastSortCameraDelta != cameraDelta) {
		mLastSortCameraDelta = cameraDelta;
		if (mSortState == SortState::Ready) {
			// The mesh has an out of date sorting, and needs to be sorted.
			_changeSortState(SortState::Ready, SortState::SortDirty);
		}
		else if (mSortState == SortState::Sorting) {
			// Sorting has already been started.  Need to redo the sort once it has finished.
			_changeSortState(SortState::Sorting, SortState::RestartSort);
		}

		// If the sort state is in RestartSort or SortDirty, the state doesn't need to change, 
		// just need to update mLastSortCameraDelta, which has already been done. Later on, 
		// this will be used to sort the faces
	}

	return;
}

void RenderChunk::sortFaces() {
	DEBUG_ASSERT(mSortState == SortState::Sorting || mSortState == SortState::RestartSort, "Invalid state.");

	const int INDICES_PER_FACE = 4;

	// Count the indices
	int indexCount = 0;
	mIndexSize = ((mNumFaces * INDICES_PER_FACE + 3) <= std::numeric_limits<uint16_t>::max()) ? sizeof(uint16_t) : sizeof(uint32_t);
	for (int rangeIdx = 0; rangeIdx < BlockRenderLayer::_RENDERLAYER_COUNT; rangeIdx++) {
		indexCount += mRenderLayerIndexRange[rangeIdx].mCount;
	}

	// Parse each range and generate a new sorted index list for it.
	mData.clear();
	mData32bit.clear();

	int idx = 0;
	for (int rangeIdx = 0; rangeIdx < BlockRenderLayer::_RENDERLAYER_COUNT; rangeIdx++) {

		// Determine which faces to render
		int faceTypesToRender[7];
		unsigned int numFaceTypesToRender = 0;
		if (rangeIdx == BlockRenderLayer::RENDERLAYER_ALPHATEST ||
			rangeIdx == BlockRenderLayer::RENDERLAYER_ALPHATEST_SINGLE_SIDE ||
			rangeIdx == BlockRenderLayer::RENDERLAYER_OPTIONAL_ALPHATEST ||
			rangeIdx == BlockRenderLayer::RENDERLAYER_SEASONS_OPTIONAL_ALPHATEST) {

			// Render all alpha tested faces
			faceTypesToRender[numFaceTypesToRender++] = Facing::EAST;
			faceTypesToRender[numFaceTypesToRender++] = Facing::WEST;
			faceTypesToRender[numFaceTypesToRender++] = Facing::UP;
			faceTypesToRender[numFaceTypesToRender++] = Facing::DOWN;
			faceTypesToRender[numFaceTypesToRender++] = Facing::SOUTH;
			faceTypesToRender[numFaceTypesToRender++] = Facing::NORTH;
			faceTypesToRender[numFaceTypesToRender++] = Facing::NOT_DEFINED;

		}
		else {
			// include front faces
			if (mLastSortCameraDelta.x <= 0)	faceTypesToRender[numFaceTypesToRender++] = Facing::EAST;
			if (mLastSortCameraDelta.x >= 0)	faceTypesToRender[numFaceTypesToRender++] = Facing::WEST;
			if (mLastSortCameraDelta.y <= 0)	faceTypesToRender[numFaceTypesToRender++] = Facing::UP;
			if (mLastSortCameraDelta.y >= 0)	faceTypesToRender[numFaceTypesToRender++] = Facing::DOWN;
			if (mLastSortCameraDelta.z <= 0)	faceTypesToRender[numFaceTypesToRender++] = Facing::SOUTH;
			if (mLastSortCameraDelta.z >= 0)	faceTypesToRender[numFaceTypesToRender++] = Facing::NORTH;

			// include all faces that don't have a defined direction
			faceTypesToRender[numFaceTypesToRender++] = Facing::NOT_DEFINED;
		}

		// record the start index for the sorted indices for this render layer 
		mSortedRenderLayerIndexRange[rangeIdx].mStart = idx;

		for (uint32_t curFaceIdx = 0; curFaceIdx < numFaceTypesToRender; curFaceIdx++) {
			int i = faceTypesToRender[curFaceIdx];
			for (uint32_t j = 0; j < mFaces[i].size(); j++) {
				// If the face is within the render layer range, add its indices to the index cache;
				const int VERTS_PER_FACE = 6;
				if (mFaces[i][j] * VERTS_PER_FACE >= (uint32_t)mRenderLayerIndexRange[rangeIdx].mStart
					&& mFaces[i][j] * VERTS_PER_FACE < (uint32_t)mRenderLayerIndexRange[rangeIdx].mStart + (uint32_t)mRenderLayerIndexRange[rangeIdx].mCount
					) {
					uint32_t baseidx = mFaces[i][j] * INDICES_PER_FACE;

					// check the index value
					DEBUG_ASSERT((((uint64_t)baseidx) + 3) < (((uint64_t)1) << (mIndexSize * 8)), "Index value overflow");
					if (mIndexSize == sizeof(uint16_t)) {
						mData.push_back(baseidx + 1);
						mData.push_back(baseidx + 2);
						mData.push_back(baseidx + 0);
						mData.push_back(baseidx + 0);
						mData.push_back(baseidx + 2);
						mData.push_back(baseidx + 3);
					}
					else {
						mData32bit.push_back(baseidx + 1);
						mData32bit.push_back(baseidx + 2);
						mData32bit.push_back(baseidx + 0);
						mData32bit.push_back(baseidx + 0);
						mData32bit.push_back(baseidx + 2);
						mData32bit.push_back(baseidx + 3);
					}

					idx += 6;
				}
			}
		}

		// record the count for the sorted indices for this render layer
		mSortedRenderLayerIndexRange[rangeIdx].mCount = idx - mSortedRenderLayerIndexRange[rangeIdx].mStart;
	}
}

RenderChunk::CutawayVisibilityBuffer& RenderChunk::getCutawayVisibilityBuffer() {
	if (!mCutawayVisibilityBuffer) {
		mCutawayVisibilityBuffer = std::make_unique<CutawayVisibilityBuffer>();
		memset(mCutawayVisibilityBuffer->data(), 0, mCutawayVisibilityBuffer->size());
	}

	return *mCutawayVisibilityBuffer;
}

void RenderChunk::render(const RenderChunkRenderParameters& renderChunkRenderParameters, const double currentTime) {
// 	TerrainLayer geometryLayer = LevelRenderer::terrainRenderLayerToGeometryLayer(renderChunkRenderParameters.layer);
// 
// 	DEBUG_ASSERT(hasLayer(geometryLayer), "Can't render this layer, pls");
// 
// 	auto& material = _chooseMaterial(renderChunkRenderParameters.layer, currentTime, renderChunkRenderParameters.forceFog);
// 
// 	auto& tex1 = (renderChunkRenderParameters.layer == TerrainLayer::EndPortal) ? endPortalTexture : atlasTexture;
// 	auto& tex2 = (renderChunkRenderParameters.layer == TerrainLayer::EndPortal) ? endPortalColorTexture : brightnessTexture;
// 
// 	if (mSortState == SortState::Ready && !renderChunkRenderParameters.forceUnsorted) {
// 		// The sorted index buffer is ready, so use it
// 		auto& ri = mSortedIndexRange[(int)geometryLayer];
// 		if (ri.mCount > 0) {
// 			mMesh.setIndexBufferUsageMode(mce::IndexBufferUsageMode::Normal);
// 			mMesh.render(material, tex1, tex2, foliageTexture, ri.mStart, ri.mCount);
// 		}
// 	}
// 	else {
// 		// Render the mesh with the default (non-sorted) index buffer
// 		auto& ri = mIndexRange[(int)geometryLayer];
// 		mMesh.setIndexBufferUsageMode(mce::IndexBufferUsageMode::ForceUseUnsorted);
// 		mMesh.render(material, tex1, tex2, foliageTexture, ri.mStart, ri.mCount);
// 	}

}

bool RenderChunk::isEmpty() const {
	return mCanRender /*&& !mMesh.isValid()*/;
}

float RenderChunk::distanceToSqr(const Entity* player) const {
	float xd = (float)(player->mPos.x - mCenter.x);
	float yd = (float)(player->mPos.y - mCenter.y);
	float zd = (float)(player->mPos.z - mCenter.z);
	return xd * xd + yd * yd + zd * zd;
}

bool RenderChunk::hasLayer(TerrainLayer r) const {
	return mIndexRange[static_cast<int>(r)].mCount > 0;
}

bool RenderChunk::isCulled() const {
	return !mVisible;
}

bool RenderChunk::isAllDark() const {
	return mAllDark;
}

float RenderChunk::getAverageBrightness() const {
	return mAverageSkyLight;
}

bool RenderChunk::canSeeChunkLookingFrom(unsigned char facingFrom, unsigned char facingTo) const {
	return mVisibility.from(facingFrom).contains(facingTo);
}

void RenderChunk::reset() {
	mVisible = false;
	mCanRender = false;
	mSkyLit = false;
	mReadyTime = 0;

// 	mMesh.reset();
}

bool RenderChunk::isVisible(int frameID) const {
	return mVisible && isVisited(frameID);
}

bool RenderChunk::wasVisibileInPastFrames() const {
	return mVisible;
}

bool RenderChunk::isBottom() const {
	return mPosition.y == 0;
}

bool RenderChunk::isFaceSortDirty() const {
	return mSortState == SortState::SortDirty;
}

bool RenderChunk::hasImmediateChange() const {
	return mImmediateChange;
}

void RenderChunk::setVisible(bool v, unsigned int frameID) {
	mVisible = v;
	mLastFrameVisited = frameID;
}

void RenderChunk::updateDistanceFromPlayer(const Vec3& from) {
	distanceFromPlayer2 = from.distanceToSqr(mCenter);
}

int RenderChunk::isVisited(unsigned int frameID) const {
	return mLastFrameVisited >= frameID;
}

bool RenderChunk::isSkyLit() const {
	return mSkyLit;
}

int RenderChunk::getPriority(const RenderChunk& r) {
	if (r.distanceFromPlayer2 == distanceFromPlayer2) {
		return 0;
	}
	else {
		return r.distanceFromPlayer2 > distanceFromPlayer2 ? -1 : 1;
	}
}

BlockPos RenderChunk::getFacing(int facing) const {
	return mCenter + (Facing::DIRECTION[facing] * RENDERCHUNK_SIDE);
}

const Vec3& RenderChunk::getFaceCenter(int facing) const {
	return mFaceCenters[facing];
}

bool RenderChunk::_tryChangeBuildState(BuildState old, BuildState current) {

	return mBuildState.compare_exchange_strong(old, current);
}

void RenderChunk::_changeBuildState(BuildState old, BuildState current) {
	bool stateChanged = _tryChangeBuildState(old, current);
	DEBUG_ASSERT(stateChanged, "Wrong state change. This is really bad. This should not be just a warning!");
	UNUSED_VARIABLE(stateChanged);
}

bool RenderChunk::_tryChangeSortState(SortState old, SortState current) {

	return mSortState.compare_exchange_strong(old, current);
}

void RenderChunk::_changeSortState(SortState old, SortState current) {
	bool stateChanged = _tryChangeSortState(old, current);
	DEBUG_ASSERT(stateChanged, "Wrong state change. This is really bad. This should not be just a warning!");
	UNUSED_VARIABLE(stateChanged);
}

void RenderChunk::setDirty(Tick now, bool immediateChange) {
	mImmediateChange |= immediateChange;

	//always store the time of last change. We could be in the dirty state, 
	//but if a change happens while rebuilding we need to rebuild again as soon as we're done
	mLastChangeTick = now;

	//store the first change only once
	if(mFirstChangeTick == Tick::MAX) {
		mFirstChangeTick = now;
	}

	// If the chunk was in the middle of being sorted, tell it to start over
	_tryChangeSortState(SortState::Sorting, SortState::RestartSort);

	//try to perform one of the valid transitions
	_tryChangeBuildState(BuildState::Ready, BuildState::Dirty) || _tryChangeBuildState(BuildState::Empty, BuildState::Dirty);
}

bool RenderChunk::isDirty(Tick now, uint32_t minTicksSinceLastChange, uint32_t minTicksSinceFirstChange) const {
	if(hasImmediateChange()) {
		minTicksSinceLastChange = 0;
	}
	return isBuildState(BuildState::Dirty) && (now > (mLastChangeTick + minTicksSinceLastChange) || now > (mFirstChangeTick + minTicksSinceFirstChange));
}

void RenderChunk::setVisibilityChanged() {
	mVisibilityChanged = true;
}

bool RenderChunk::isPending() const {
	return mBuildState == BuildState::Dirty || mBuildState == BuildState::Building;
}

bool RenderChunk::isReady() {
	return mCanRender;
}

#ifdef DEBUG_STATS
int RenderChunk::getPolygonCount(TerrainLayer layer) const {
	return mPolyCount[enum_cast(layer)];
}
#endif // DEBUG_STATS

bool RenderChunk::isBuildState(BuildState s) const {
	return mBuildState == s;
}

bool RenderChunk::isSortState(SortState s) const {
	return mSortState == s;
}

void RenderChunk::setMaterialsBasedOnCameraUnderLiquid(const bool isUnderwater, const bool isUnderLava) {

// 	sFogMaterialMap[enum_cast(TerrainLayer::Opaque)] = mce::RenderMaterialGroup::switchable.getMaterial(isUnderLava ? "terrain_opaque_fog_no_cull" : "terrain_opaque_fog");
// 
// 	sMaterialMap[enum_cast(TerrainLayer::Blend)] = mce::RenderMaterialGroup::switchable.getMaterial(isUnderwater ? "terrain_blend_below" : "terrain_blend");
// 	sFogMaterialMap[enum_cast(TerrainLayer::Blend)] = mce::RenderMaterialGroup::switchable.getMaterial(isUnderwater ? "terrain_blend_below_fog" : "terrain_blend_fog");
// 	sMaterialMap[enum_cast(TerrainLayer::Water)] = mce::RenderMaterialGroup::switchable.getMaterial(isUnderwater ? "terrain_water_below" : "terrain_water");
// 	sFogMaterialMap[enum_cast(TerrainLayer::Water)] = mce::RenderMaterialGroup::switchable.getMaterial(isUnderwater ? "terrain_water_below_fog" : "terrain_water_fog");
}

BlockPos RenderChunk::getPosition() const {
	return mPosition;
}

void RenderChunk::setPosition(BlockPos Position) {
	mPosition = Position;
}

BlockPos RenderChunk::getCenter() const {
	return mCenter;
}

void RenderChunk::setCenter(BlockPos Center) {
	mCenter = Center;
}

Bounds RenderChunk::getDataBounds() const {
	return mDataBounds;
}

void RenderChunk::setDataBounds(Bounds DataBounds) {
	mDataBounds = DataBounds;
}

AABB RenderChunk::getBoundingBox() const {
	return mBoundingBox;
}

void RenderChunk::setBoundingBox(AABB BoundingBox) {
	mBoundingBox = BoundingBox;
}
