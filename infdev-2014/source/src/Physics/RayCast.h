#pragma once

#include <limits>

#include <stdafx.h>
#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>
#include <BulletCollision/CollisionDispatch/btCollisionObject.h>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <glm/vec3.hpp>

#include "General/Vector3.h"
#include "Physics/PhysicsTypes.h"

class btCollisionObject;

namespace Esteem
{
	class Collidable;

	struct RayCastInfo
	{
		bool hasHit;
		Vector3 origin;

		Vector3 ray;
		Vector3 rayInverse;

		Vector3 hitPoint;
		Vector3d hitVoxel;
		Vector3 hitNormal;
		float distanceSquared;
		Collidable* hitObject;

		int filterMask;
		int flags;

		RayCastInfo(const Vector3& origin, const Vector3& ray, int filterMask = ~Collision::FilterGroups::SensorTrigger, int flags = 0)
			: hasHit(false)
			, hitPoint(0, 0, 0)
			, hitVoxel(std::numeric_limits<double>::infinity())
			, hitNormal(0, 0, 0)
			, distanceSquared(Vector3::Dot(ray, ray))
			, hitObject(nullptr)
			, origin(origin)
			, ray(ray)
			, rayInverse(-ray)
			, filterMask(filterMask)
			, flags(flags)
		{
		}
	};

	struct ShapeCastInfo
	{
		bool hasHit;
		glm::mat4 origin;
		glm::mat4 end;

		uint8 shape;

		glm::vec3 hitPoint;
		glm::dvec3 hitVoxel;
		glm::vec3 hitNormal;

		float distanceSquared;
		btCollisionObject* hitObject;

		ShapeCastInfo(const glm::mat4& origin, const glm::mat4& end, uint8 shape)
			: hasHit(false)
			, hitPoint(0, 0, 0)
			, hitVoxel(std::numeric_limits<double>::infinity())
			, hitNormal(0, 0, 0)
			, distanceSquared(std::numeric_limits<float>::max())
			, hitObject(nullptr)
			, origin(origin)
			, end(end)
			, shape(shape)
		{
		}
	};

	template<class R>
	struct CustomRayResultInfo : public R
	{
		glm::dvec3 objPosition;

		template<typename... Args>
		CustomRayResultInfo(Args&&... arguments)
			: R(std::forward<Args>(arguments)...)
			, objPosition()
		{ }

		bool needsCollision(btBroadphaseProxy* proxy0) const override
		{
			return R::needsCollision(proxy0)
				&& static_cast<btCollisionObject*>(proxy0->m_clientObject)->getCollisionShape()->getShapeType() != CUSTOM_CONCAVE_SHAPE_TYPE;
		}
	};
}