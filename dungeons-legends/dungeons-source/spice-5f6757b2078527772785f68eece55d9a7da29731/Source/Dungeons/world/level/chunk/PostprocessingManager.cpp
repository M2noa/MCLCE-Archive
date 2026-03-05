/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "CommonTypes.h"
#include "world/level/chunk/PostprocessingManager.h"
#include "world/level/BlockPos.h"
#include "util/Bounds.h"

PostprocessingManager PostprocessingManager::instance;

PostprocessingManager::PostprocessingManager() :
	mNeedsSynchronization(std::thread::hardware_concurrency() > 2){

}

PostprocessingManager::~PostprocessingManager(){

}

PostprocessingManager::Owns PostprocessingManager::tryLock(const ChunkPos& center){
	if (mNeedsSynchronization) {
		LockGuard lock(mManagerMutex);

		ChunkPos min(center.x - 1, center.z - 1);
		ChunkPos max(center.x + 1, center.z + 1);
		if (mAcquired.size() > 0) {
			for (ChunkPos p = min; p.x <= max.x; ++p.x) {
				for (p.z = min.z; p.z <= max.z; ++p.z) {
					if (mAcquired.count(p) > 0) {//AWWW one was taken, no dice
						return Owns();
					}
				}
			}
		}

		//got it! register that we are using it
		for (ChunkPos p = min; p.x <= max.x; ++p.x) {
			for (p.z = min.z; p.z <= max.z; ++p.z) {
				mAcquired.insert(p);
			}
		}
	}

	return Owns(center);
}

void PostprocessingManager::_release(const ChunkPos& center){
	if (mNeedsSynchronization) {
		LockGuard lock(mManagerMutex);

		ChunkPos min(center.x - 1, center.z - 1);
		ChunkPos max(center.x + 1, center.z + 1);

		for (ChunkPos p = min; p.x <= max.x; ++p.x) {
			for (p.z = min.z; p.z <= max.z; ++p.z) {
				mAcquired.erase(p);
			}
		}
	}
}

PostprocessingManager::Owns::Owns(const ChunkPos& p) 
	: mPosition(p) {
}

PostprocessingManager::Owns::Owns() 
	: mPosition(ChunkPos::INVALID) {
}

PostprocessingManager::Owns::~Owns() {
	if (mPosition != ChunkPos::INVALID) {
		PostprocessingManager::instance._release(mPosition);
	}
}

PostprocessingManager::Owns::operator bool() const {
	return mPosition != ChunkPos::INVALID;
}
