#include "FrustumCullingChecker.h"
#include "Math/Math.h"

namespace Esteem
{
	bool FrustumCullingChecker::SingleObjectCheck(const RenderObject* object)
	{
		return PointInFrustum(object->GetPosition());
	}

	void FrustumCullingChecker::RangeOfObjectsCheck()
	{

	}

	FrustumCullingChecker::~FrustumCullingChecker()
	{

	}

	void FrustumCullingChecker::CalculateFrustum(const glm::mat4& cameraVP)
	{		
		planes[LEFT] = Math::NormalizeFrustumPlane({
			cameraVP[0][3] + cameraVP[0][0],
			cameraVP[1][3] + cameraVP[1][0],
			cameraVP[2][3] + cameraVP[2][0],
			cameraVP[3][3] + cameraVP[3][0]
		});

		planes[RIGHT] = Math::NormalizeFrustumPlane({
			cameraVP[0][3] - cameraVP[0][0],
			cameraVP[1][3] - cameraVP[1][0],
			cameraVP[2][3] - cameraVP[2][0],
			cameraVP[3][3] - cameraVP[3][0]
		});

		planes[BOTTOM] = Math::NormalizeFrustumPlane({
			cameraVP[0][3] + cameraVP[0][1],
			cameraVP[1][3] + cameraVP[1][1],
			cameraVP[2][3] + cameraVP[2][1],
			cameraVP[3][3] + cameraVP[3][1]
		});
		
		planes[TOP] = Math::NormalizeFrustumPlane({
			cameraVP[0][3] - cameraVP[0][1],
			cameraVP[1][3] - cameraVP[1][1],
			cameraVP[2][3] - cameraVP[2][1],
			cameraVP[3][3] - cameraVP[3][1]
		});
		
		planes[FRONT] = Math::NormalizeFrustumPlane({
			cameraVP[0][3] + cameraVP[0][2],
			cameraVP[1][3] + cameraVP[1][2],
			cameraVP[2][3] + cameraVP[2][2],
			cameraVP[3][3] + cameraVP[3][2]
		});
		
		planes[BACK] = Math::NormalizeFrustumPlane({
			cameraVP[0][3] - cameraVP[0][2],
			cameraVP[1][3] - cameraVP[1][2],
			cameraVP[2][3] - cameraVP[2][2],
			cameraVP[3][3] - cameraVP[3][2]
		});
	}

	bool FrustumCullingChecker::PointInFrustum(const glm::vec3& point)
	{
		for (int i = 0; i < 6; i++)
		{
			if (planes[i].x * point.x + planes[i].y * point.y + planes[i].z * point.z + planes[i].w <= 0)
				return false;
		}

		return true;
	}

	bool FrustumCullingChecker::SphereInFrustum(const glm::vec3& position, float radius)
	{
		for (const auto& plane : planes)
		{
			if (plane.x * position.x + plane.y * position.y + plane.z * position.z + plane.w <= -radius)
				return false;
		}

		return true;
	}

	bool FrustumCullingChecker::CubeInFrustum(const glm::vec3& min, const glm::vec3& max)
	{
		for (const auto& plane : planes)
		{
			if (plane.x * min.x + plane.y * min.y + plane.z * min.z + plane.w <= 0.0
				&& plane.x * max.x + plane.y * min.y + plane.z * min.z + plane.w <= 0.0
				&& plane.x * min.x + plane.y * max.y + plane.z * min.z + plane.w <= 0.0
				&& plane.x * max.x + plane.y * max.y + plane.z * min.z + plane.w <= 0.0
				&& plane.x * min.x + plane.y * min.y + plane.z * max.z + plane.w <= 0.0
				&& plane.x * max.x + plane.y * min.y + plane.z * max.z + plane.w <= 0.0
				&& plane.x * min.x + plane.y * max.y + plane.z * max.z + plane.w <= 0.0
				&& plane.x * max.x + plane.y * max.y + plane.z * max.z + plane.w <= 0.0)
				return false;
		}

		return true;
	}

	bool FrustumCullingChecker::CubeInFrustum(const glm::vec3& position, float halfSize)
	{
		return CubeInFrustum(position - halfSize, position + halfSize);		
	}

	bool FrustumCullingChecker::CubeInFrustum(const AxisAlignedBox& aab)
	{
		return CubeInFrustum(aab.begin, aab.end);
	}
}