#pragma once

#include "GameRuleDefinition.h"

class NamedAreaRuleDefinition : public GameRuleDefinition
{
private:
	wstring m_name;
	AABB *m_area;

public:
	NamedAreaRuleDefinition();
	~NamedAreaRuleDefinition();

	virtual void writeAttributes(DataOutputStream *dos, UINT numAttributes);

	virtual ConsoleGameRules::EGameRuleType getActionType() { return ConsoleGameRules::eGameRuleType_NamedArea; }
	
	virtual void addAttribute(const wstring &attributeName, const wstring &attributeValue);

	AABB *getArea() { return m_area; }
	wstring getName() { return m_name; }
};
