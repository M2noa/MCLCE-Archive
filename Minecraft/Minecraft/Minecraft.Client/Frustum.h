#pragma once
#include "FrustumData.h"

class Frustum : public FrustumData
{
private:
	static Frustum *frustum;

public:
	static FrustumData *getFrustum();

    ///////////////////////////////// NORMALIZE PLANE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
    /////
    /////	This normalizes a plane (A side) from a given frustum.
    /////
    ///////////////////////////////// NORMALIZE PLANE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

private:
    void normalizePlane(float **frustum, int side);

    FloatBuffer *_proj;
    FloatBuffer *_modl;
    FloatBuffer *_clip;

    void calculateFrustum();

	Frustum();
	~Frustum();
};