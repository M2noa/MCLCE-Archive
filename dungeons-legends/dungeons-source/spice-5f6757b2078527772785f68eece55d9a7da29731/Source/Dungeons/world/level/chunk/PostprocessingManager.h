/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "common_header.h"

#include "world/level/ChunkPos.h"

class PostprocessingManager {
public:
	struct Owns {
		Owns();
		explicit Owns(const ChunkPos& p);

		~Owns();

		explicit operator bool() const;

	private:
		ChunkPos mPosition;
	};

	static PostprocessingManager instance;

	PostprocessingManager();

	~PostprocessingManager();

	Owns tryLock(const ChunkPos& center);

private:

	std::mutex mManagerMutex;
	std::unordered_set<ChunkPos> mAcquired;
	const bool mNeedsSynchronization;

	void _release(const ChunkPos& center);
};
