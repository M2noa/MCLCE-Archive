#pragma once

#include "XboxStructureActionPlaceBlock.h"

class StructurePiece;
class Level;
class BoundingBox;
class GRFObject;

class XboxStructureActionPlaceSpawner : public XboxStructureActionPlaceBlock
{
private:
	wstring m_entityId;
public:
	XboxStructureActionPlaceSpawner();
	~XboxStructureActionPlaceSpawner();

	virtual ConsoleGameRules::EGameRuleType getActionType() { return ConsoleGameRules::eGameRuleType_PlaceSpawner; }
	
	virtual void writeAttributes(DataOutputStream *dos, UINT numAttrs);
	virtual void addAttribute(const wstring &attributeName, const wstring &attributeValue);

	bool placeSpawnerInLevel(StructurePiece *structure, Level *level, BoundingBox *chunkBB);
};