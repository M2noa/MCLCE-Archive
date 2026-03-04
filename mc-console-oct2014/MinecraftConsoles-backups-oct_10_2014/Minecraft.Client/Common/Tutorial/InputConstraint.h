#pragma once

#include "TutorialConstraint.h"

class InputConstraint : public TutorialConstraint
{
private:
	int m_inputMapping; // Should be one of the EControllerActions
public:	
	virtual ConstraintType getType() { return e_ConstraintInput; }

	InputConstraint(int mapping) : TutorialConstraint(-1), m_inputMapping( mapping ) {}

	virtual bool isMappingConstrained(int iPad, int mapping);
};