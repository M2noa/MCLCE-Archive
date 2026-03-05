#pragma once

using namespace std;

class LevelGenerationOptions;

class LevelGenerators
{
private:
	vector<LevelGenerationOptions *> m_levelGenerators;

public:
	LevelGenerators();

	void addLevelGenerator(const wstring &displayName, LevelGenerationOptions *generator);
	void removeLevelGenerator(LevelGenerationOptions *generator);

	vector<LevelGenerationOptions *> *getLevelGenerators() { return &m_levelGenerators; }
};