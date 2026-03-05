#pragma once

#include "./ICullingChecker.h"
#include "Memory/Partitioning/Spatial/AxisAlignedBox.h"

namespace Esteem
{
	enum FrustumSide
	{
		RIGHT = 0,		// The RIGHT side of the frustum
		LEFT = 1,		// The LEFT	 side of the frustum
		BOTTOM = 2,		// The BOTTOM side of the frustum
		TOP = 3,		// The TOP side of the frustum
		BACK = 4,		// The BACK	side of the frustum
		FRONT = 5		// The FRONT side of the frustum
	};
	
	/// \brief Frustum culling checks
	class FrustumCullingChecker : public ICullingChecker
	{
	private:
		glm::vec4 planes[6];

	public:
		virtual ~FrustumCullingChecker();

		virtual bool SingleObjectCheck(const RenderObject* object);
		virtual void RangeOfObjectsCheck();


		// \brief Call this every time the camera moves, to update the frustum
		void CalculateFrustum(const glm::mat4& cameraMVP);

		// \brief This takes a 3D point and returns TRUE if it's inside of the frustum
		bool PointInFrustum(const glm::vec3& point);

		// \brief This takes a 3D point and a radius and returns TRUE if the sphere is inside of the frustum
		bool SphereInFrustum(const glm::vec3& position, float radius);

		// \brief This checks if a box is in the frustum
		bool CubeInFrustum(const glm::vec3& min, const glm::vec3& max);

		// \brief This takes the center and half the length of the cube.
		bool CubeInFrustum(const glm::vec3& position, float halfSize);

		// \brief This takes the center and half the length of the cube.
		bool CubeInFrustum(const AxisAlignedBox& aab);
	};
}