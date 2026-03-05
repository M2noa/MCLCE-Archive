#pragma once
#include "Culler.h"

class AllowAllCuller
{
public:
	virtual bool isVisible(AABB *bb);
	virtual bool cubeInFrustum(double x0, double y0, double z0, double x1, double y1, double z1);
	virtual bool cubeFullyInFrustum(double x0, double y0, double z0, double x1, double y1, double z1);
	virtual void prepare(double xOff, double yOff, double zOff);
};