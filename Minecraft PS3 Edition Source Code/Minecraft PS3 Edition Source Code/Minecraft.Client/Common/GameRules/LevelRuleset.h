#pragma once

#include "CompoundGameRuleDefinition.h"

class NamedAreaRuleDefinition;

class LevelRuleset : public CompoundGameRuleDefinition
{
private:
	vector<NamedAreaRuleDefinition *> m_areas;
	StringTable *m_stringTable;
public:
	LevelRuleset();
	~LevelRuleset();

	virtual void getChildren(vector<GameRuleDefinition *> *children);
	virtual GameRuleDefinition *addChild(ConsoleGameRules::EGameRuleType ruleType);

	virtual ConsoleGameRules::EGameRuleType getActionType() { return ConsoleGameRules::eGameRuleType_LevelRules; }

	void loadStringTable(StringTable *table);
	LPCWSTR getString(const wstring &key);

	AABB *getNamedArea(const wstring &areaName);

	StringTable *getStringTable() { return m_stringTable; }
};