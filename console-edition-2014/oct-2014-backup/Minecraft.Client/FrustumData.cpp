#include "stdafx.h"
#include "FrustumData.h"    

float** m_Frustum;


FrustumData::FrustumData()
{
	m_Frustum = new float *[16];
	for( int i = 0; i < 16; i++ ) m_Frustum[i] = new float[16];
    proj = floatArray( 16 );
    modl = floatArray( 16 );
    clip = floatArray( 16 );
}

FrustumData::~FrustumData()
{
	delete[] proj.data;
	delete[] modl.data;
	delete[] clip.data;
	for( int i = 0; i < 16; i++ ) delete[] m_Frustum[i];
	delete[] m_Frustum;
}

bool FrustumData::pointInFrustum(float x, float y, float z)
{
    for (int i = 0; i < 6; i++)
    {
        if (m_Frustum[i][A] * x + m_Frustum[i][B] * y + m_Frustum[i][C] * z + m_Frustum[i][D] <= 0)
        {
            return false;
        }
    }
    
    return true;
}

bool FrustumData::sphereInFrustum(float x, float y, float z, float radius)
{
	for (int i = 0; i < 6; i++)
	{
		if (m_Frustum[i][A] * x + m_Frustum[i][B] * y + m_Frustum[i][C] * z + m_Frustum[i][D] <= -radius)
		{
			return false;
		}
	}
    
	return true;
}

bool FrustumData::cubeFullyInFrustum(double x1, double y1, double z1, double x2, double y2, double z2)
{
    for (int i = 0; i < 6; i++)
    {
        if (!(m_Frustum[i][A] * (x1) + m_Frustum[i][B] * (y1) + m_Frustum[i][C] * (z1) + m_Frustum[i][D] > 0)) return false;
        if (!(m_Frustum[i][A] * (x2) + m_Frustum[i][B] * (y1) + m_Frustum[i][C] * (z1) + m_Frustum[i][D] > 0)) return false;
        if (!(m_Frustum[i][A] * (x1) + m_Frustum[i][B] * (y2) + m_Frustum[i][C] * (z1) + m_Frustum[i][D] > 0)) return false;
        if (!(m_Frustum[i][A] * (x2) + m_Frustum[i][B] * (y2) + m_Frustum[i][C] * (z1) + m_Frustum[i][D] > 0)) return false;
        if (!(m_Frustum[i][A] * (x1) + m_Frustum[i][B] * (y1) + m_Frustum[i][C] * (z2) + m_Frustum[i][D] > 0)) return false;
        if (!(m_Frustum[i][A] * (x2) + m_Frustum[i][B] * (y1) + m_Frustum[i][C] * (z2) + m_Frustum[i][D] > 0)) return false;
        if (!(m_Frustum[i][A] * (x1) + m_Frustum[i][B] * (y2) + m_Frustum[i][C] * (z2) + m_Frustum[i][D] > 0)) return false;
        if (!(m_Frustum[i][A] * (x2) + m_Frustum[i][B] * (y2) + m_Frustum[i][C] * (z2) + m_Frustum[i][D] > 0)) return false;
    }
    
    return true;
}

bool FrustumData::cubeInFrustum(double x1, double y1, double z1, double x2, double y2, double z2)
{
    for (int i = 0; i < 6; i++)
    {
        if (m_Frustum[i][A] * (x1) + m_Frustum[i][B] * (y1) + m_Frustum[i][C] * (z1) + m_Frustum[i][D] > 0) continue;
        if (m_Frustum[i][A] * (x2) + m_Frustum[i][B] * (y1) + m_Frustum[i][C] * (z1) + m_Frustum[i][D] > 0) continue;
        if (m_Frustum[i][A] * (x1) + m_Frustum[i][B] * (y2) + m_Frustum[i][C] * (z1) + m_Frustum[i][D] > 0) continue;
        if (m_Frustum[i][A] * (x2) + m_Frustum[i][B] * (y2) + m_Frustum[i][C] * (z1) + m_Frustum[i][D] > 0) continue;
        if (m_Frustum[i][A] * (x1) + m_Frustum[i][B] * (y1) + m_Frustum[i][C] * (z2) + m_Frustum[i][D] > 0) continue;
        if (m_Frustum[i][A] * (x2) + m_Frustum[i][B] * (y1) + m_Frustum[i][C] * (z2) + m_Frustum[i][D] > 0) continue;
        if (m_Frustum[i][A] * (x1) + m_Frustum[i][B] * (y2) + m_Frustum[i][C] * (z2) + m_Frustum[i][D] > 0) continue;
        if (m_Frustum[i][A] * (x2) + m_Frustum[i][B] * (y2) + m_Frustum[i][C] * (z2) + m_Frustum[i][D] > 0) continue;
    
        return false;
    }
    
    return true;
}

bool FrustumData::isVisible(AABB *aabb)
{
	return cubeInFrustum(aabb->x0, aabb->y0, aabb->z0, aabb->x1, aabb->y1, aabb->z1);
}