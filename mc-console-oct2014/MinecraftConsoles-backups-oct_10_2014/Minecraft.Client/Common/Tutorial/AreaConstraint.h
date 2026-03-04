#pragma once

#include "TutorialConstraint.h"

class AABB;

class AreaConstraint : public TutorialConstraint
{
private:
	AABB *movementArea;
	AABB *messageArea;
	bool contains; // If true we must stay in this area, if false must stay out of this area
	bool m_restrictsMovement;

public:
	virtual ConstraintType getType() { return e_ConstraintArea; }

	AreaConstraint( int descriptionId, double x0, double y0, double z0, double x1, double y1, double z1, bool contains = true, bool restrictsMovement =true );
	~AreaConstraint();

	virtual bool isConstraintSatisfied(int iPad);
	virtual bool isConstraintRestrictive(int iPad);
	virtual bool canMoveToPosition(double xo, double yo, double zo, double xt, double yt, double zt);
};