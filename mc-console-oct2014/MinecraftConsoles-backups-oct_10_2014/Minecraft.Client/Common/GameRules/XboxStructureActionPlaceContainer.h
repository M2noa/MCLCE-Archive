#pragma once

#include "XboxStructureActionPlaceBlock.h"

class AddItemRuleDefinition;
class StructurePiece;
class Level;
class BoundingBox;

class XboxStructureActionPlaceContainer : public XboxStructureActionPlaceBlock
{
private:
	vector<AddItemRuleDefinition *> m_items;
public:
	XboxStructureActionPlaceContainer();
	~XboxStructureActionPlaceContainer();

	virtual ConsoleGameRules::EGameRuleType getActionType() { return ConsoleGameRules::eGameRuleType_PlaceContainer; }
	
	virtual void getChildren(vector<GameRuleDefinition *> *children);
	virtual GameRuleDefinition *addChild(ConsoleGameRules::EGameRuleType ruleType);
	
	// 4J-JEV: Super class handles attr-facing fine.
	//virtual void writeAttributes(DataOutputStream *dos, UINT numAttributes);
	
	virtual void addAttribute(const wstring &attributeName, const wstring &attributeValue);

	bool placeContainerInLevel(StructurePiece *structure, Level *level, BoundingBox *chunkBB);
};