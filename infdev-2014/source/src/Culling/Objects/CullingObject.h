#pragma once

#include <deque>
#include <cppu/stor/lock/deque.h>

//#include 
#include "Rendering/Objects/RenderObject.h"
#include "World/Objects/Transform.h"
#include "Memory/Partitioning/Spatial/SpatialObject.h"

#include "General/Delegate.h"

namespace Esteem
{
	class Culling;
		
	// TODO: check if this object can be made smaller with CPU caching in mind
	class CullingObject : public Partitioning::SpatialObject
	{
	friend class Culling;

	public:
		enum Type : uint8
		{
			STATIC,
			DYNAMIC,
			VOXEL,

			COUNT
		};

	private:
		std::vector<cgc::strong_ptr<RenderObject>> renderObjects;

	public:
		Type type;

		CullingObject(const std::vector<cgc::strong_ptr<RenderObject>>& renderObjects, glm::vec3 center, glm::vec3 halfVolume, Type type = Type::DYNAMIC);
		CullingObject(std::vector<cgc::strong_ptr<RenderObject>>&& renderObjects, glm::vec3 center, glm::vec3 halfVolume, Type type = Type::DYNAMIC);
		
		/// \brief Create a new CullingObject and calculate AABB from the transform values
		/// Orientation and scale will be used to calculate the AABB
		CullingObject(const std::vector<cgc::strong_ptr<RenderObject>>& renderObjects, glm::vec3 modelCenter, glm::vec3 modelHalfVolume, Transform* transform, Type type = Type::DYNAMIC);
		CullingObject(std::vector<cgc::strong_ptr<RenderObject>>&& renderObjects, glm::vec3 modelCenter, glm::vec3 modelHalfVolume, Transform* transform, Type type = Type::DYNAMIC);

		void SetAABBFromTransform(const glm::vec3& center, const glm::vec3& halfVolume, const Transform* transform);

		bool IsDynamic() const;

		const std::vector<cgc::strong_ptr<RenderObject>>& GetRenderObjects() const;
		void SetRenderObjects(const std::vector<cgc::strong_ptr<RenderObject>>& copy);
		void SetRenderObjects(std::vector<cgc::strong_ptr<RenderObject>>&& swap);
		void ClearRenderObjects();

		void TransformUpdate();

		~CullingObject() = default;
	};
}

#include "./CullingObject.inl"