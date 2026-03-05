/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#if !defined(DEMO_MODE)

#include "world/level/storage/LevelStorageSource.h"

class ProgressListener;

class ExternalFileLevelStorageSource : public LevelStorageSource {
public:

	ExternalFileLevelStorageSource(const std::string& externalPath);

	const std::string& getName() const override;
	void getLevelList(LevelSummaryList& dest) override;

	Unique<LevelStorage> createLevelStorage(const std::string& levelId) override;
	LevelData getLevelData(const std::string& levelId) override;
	virtual void setLevelData(const std::string& levelId, const LevelData& data) override;

	bool isNewLevelIdAcceptable(const std::string& levelId) override;

	void clearAll() override;

	void deleteLevel(const std::string& levelId) override;
	std::string _getFullPath(const std::string& levelID) const;
	bool renameLevel(const std::string& levelId, const std::string& newLevelName) override;
	virtual void copyLevelFromFilePath(const std::string& levelPath) override;

	bool isConvertible(const std::string& levelId) override;
	bool requiresConversion(const std::string& levelId) override;
	bool convertLevel(const std::string& levelId, ProgressListener* progress) override;

	const std::string& getBasePath() const override;

	static void addLevelSummaryIfExists(LevelSummaryList& dest, const std::string& directory);

private:

	
	std::string _getFullPath(const std::string& levelId);

	std::string mBasePath;
};

#endif
