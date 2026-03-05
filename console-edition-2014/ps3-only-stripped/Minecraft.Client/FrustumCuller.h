#include "stdafx.h"
#include "Culler.h"
#include "Frustum.h"

class FrustumCuller : public Culler
{    
public:
	FrustumData *frustum;
	FrustumCuller();
	double xOff, yOff, zOff;
public:
	virtual void prepare(double xOff, double yOff, double zOff);
    virtual bool cubeFullyInFrustum(double x0, double y0, double z0, double x1, double y1, double z1);
    virtual bool cubeInFrustum(double x0, double y0, double z0, double x1, double y1, double z1);
    virtual bool isVisible(AABB *bb);
};
