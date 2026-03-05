/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "CommonTypes.h"

#include "world/level/storage/LevelSummary.h"

class ProgressListener;
class LevelData;
class LevelStorage;

class LevelStorageSource {
public:

	virtual ~LevelStorageSource() {}

	virtual const std::string& getName() const = 0;
	virtual void getLevelList(LevelSummaryList& dest) = 0;

	virtual LevelData getLevelData(const std::string& levelId) = 0;
	virtual void setLevelData(const std::string& levelId, const LevelData& data) = 0;

	virtual Unique<LevelStorage> createLevelStorage(const std::string& levelId) = 0;

	/**
	 * Tests if a levelId can be used to store a level. For example, a levelId
	 * can't be called COM1 on Windows systems, because that is a reserved file
	 * handle.
	 *
	 * Also, a new levelId may not overwrite an existing one.
	 */
	virtual bool isNewLevelIdAcceptable(const std::string& levelId) = 0;

	virtual void clearAll() = 0;
	virtual void deleteLevel(const std::string& levelId) = 0;
	virtual bool renameLevel(const std::string& levelId, const std::string& newLevelName) = 0;
	virtual void copyLevelFromFilePath(const std::string& levelPath) = 0;

	virtual bool isConvertible(const std::string& levelId) = 0;
	virtual bool requiresConversion(const std::string& levelId) = 0;
	virtual bool convertLevel(const std::string& levelId, ProgressListener* progress) = 0;

	virtual const std::string& getBasePath() const = 0;

};
