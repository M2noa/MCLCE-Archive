#include "stdafx.h"
#include "FrustumCuller.h"

FrustumCuller::FrustumCuller()
{
	frustum = Frustum::getFrustum();
}

void FrustumCuller::prepare(double xOff, double yOff, double zOff)
{
    this->xOff = xOff;
    this->yOff = yOff;
    this->zOff = zOff;
}

bool FrustumCuller::cubeFullyInFrustum(double x0, double y0, double z0, double x1, double y1, double z1)
{
	return frustum->cubeFullyInFrustum(x0 - xOff, y0 - yOff, z0 - zOff, x1 - xOff, y1 - yOff, z1 - zOff);
}

bool FrustumCuller::cubeInFrustum(double x0, double y0, double z0, double x1, double y1, double z1)
{
	return frustum->cubeInFrustum(x0 - xOff, y0 - yOff, z0 - zOff, x1 - xOff, y1 - yOff, z1 - zOff);
}

bool FrustumCuller::isVisible(AABB *bb)
{
	return cubeInFrustum(bb->x0, bb->y0, bb->z0, bb->x1, bb->y1, bb->z1);
}