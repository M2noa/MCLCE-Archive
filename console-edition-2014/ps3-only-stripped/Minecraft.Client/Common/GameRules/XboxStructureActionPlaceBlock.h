#pragma once
#include "ConsoleGenerateStructureAction.h"

class StructurePiece;
class Level;
class BoundingBox;

class XboxStructureActionPlaceBlock : public ConsoleGenerateStructureAction
{
protected:
	int m_x, m_y, m_z, m_tile, m_data;
public:
	XboxStructureActionPlaceBlock();

	virtual ConsoleGameRules::EGameRuleType getActionType() { return ConsoleGameRules::eGameRuleType_PlaceBlock; }

	virtual int getEndX() { return m_x; }
	virtual int getEndY() { return m_y; }
	virtual int getEndZ() { return m_z; }

	virtual void writeAttributes(DataOutputStream *dos, UINT numAttrs);
	virtual void addAttribute(const wstring &attributeName, const wstring &attributeValue);

	bool placeBlockInLevel(StructurePiece *structure, Level *level, BoundingBox *chunkBB);
};