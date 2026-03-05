/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

#include "common_header.h"

#ifdef DEBUG_STATS

#include "world/level/ChunkPos.h"

class DebugStats {

public:

	enum class AVCStats {
		None,
		Visibility,
		UpgradedChunks,

		_Count
	};

	static DebugStats instance;

	std::vector<ChunkPos> mLiveSavedChunks, mRebuiltChunks;
	std::unordered_map<ChunkPos, bool> mRenderedChunks;
	std::unordered_set<ChunkPos> mUpdatedChunks;

	bool mBiomeMapDisplayVisible = false,
		mFrameTimerDisplayVisible = false;

	int mDisplayWidth = 0;
	int mDisplayHeight = 0;
	int mViewDistance = 0;

	AVCStats mAVC = AVCStats::None;
	bool mFlyFast = false;

	void toggleBiomeMapDisplay() {
		mBiomeMapDisplayVisible = !mBiomeMapDisplayVisible;
	}

	void toggleFrameTimerDisplay() {
		mFrameTimerDisplayVisible = !mFrameTimerDisplayVisible;
	}

	void toggleFlyFast() {
		mFlyFast = !mFlyFast;
	}

	void setDisplayResolution(int width, int height) {
		mDisplayWidth = width;
		mDisplayHeight = height;
	}

	void setViewDistance(int viewDistance) {
		mViewDistance = viewDistance;
	}

	void nextAVCStat();

	void fillSystemInfo(std::vector<std::string> &outputInfo);
	std::string getOSString();
	std::string getSystemMemString();
	std::string getCPUString();
	std::string getResolutionString();
	std::string getGPUString();
	std::string getMSAALevelString();
	std::string getGPUMemoryString();
};

#endif
