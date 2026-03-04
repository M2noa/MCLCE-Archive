#include "stdafx.h"
#include "AllowAllCuller.h"	

bool AllowAllCuller::isVisible(AABB *bb)
{
	return true;
}

bool AllowAllCuller::cubeInFrustum(double x0, double y0, double z0, double x1, double y1, double z1)
{
	return true;
}

bool AllowAllCuller::cubeFullyInFrustum(double x0, double y0, double z0, double x1, double y1, double z1)
{
	return true;
}

void AllowAllCuller::prepare(double xOff, double yOff, double zOff)
{
}