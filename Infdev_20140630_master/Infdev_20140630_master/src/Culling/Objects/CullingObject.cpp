#include "CullingObject.h"

#include <algorithm>

#include "Culling/Culling.h"

namespace Esteem
{
	CullingObject::CullingObject(const std::vector<cgc::strong_ptr<RenderObject>>& renderObjects, glm::vec3 center, glm::vec3 halfVolume, Type type)
		: Partitioning::SpatialObject(center)
		, renderObjects(renderObjects)
		, type(type)
	{
		SetHalfVolume(halfVolume);
		RecalculateAABB();
	}

	CullingObject::CullingObject(std::vector<cgc::strong_ptr<RenderObject>>&& renderObjects, glm::vec3 center, glm::vec3 halfVolume, Type type)
		: Partitioning::SpatialObject(center)
		, renderObjects(std::move(renderObjects))
		, type(type)
	{
		SetHalfVolume(halfVolume);
		RecalculateAABB();
	}

	CullingObject::CullingObject(const std::vector<cgc::strong_ptr<RenderObject>>& renderObjects, glm::vec3 modelCenter, glm::vec3 modelHalfVolume, Transform* transform, Type type)
		: Partitioning::SpatialObject()
		, renderObjects(renderObjects)
		, type(type)
	{
		SetAABBFromTransform(modelCenter, modelHalfVolume, transform);
	}

	CullingObject::CullingObject(std::vector<cgc::strong_ptr<RenderObject>>&& renderObjects, glm::vec3 modelCenter, glm::vec3 modelHalfVolume, Transform* transform, Type type)
		: Partitioning::SpatialObject()
		, renderObjects(std::move(renderObjects))
		, type(type)
	{
		SetAABBFromTransform(modelCenter, modelHalfVolume, transform);
	}

	/*void CullingObject::RecalculateAABB()
	{
		glm::vec3 p0 = glm::abs((volume.x * entity->GetRight()) + (volume.y * entity->GetUp()) + (volume.z * entity->GetForward()));
		glm::vec3 p1 = glm::abs(p0 - (volume.x * entity->GetRight() * 2.f));
		glm::vec3 p2 = glm::abs(p0 - (volume.y * entity->GetUp() * 2.f));
		glm::vec3 p3 = glm::abs(p1 - (volume.y * entity->GetUp() * 2.f));

		volume.x = std::max({ p0.x, p1.x, p2.x, p3.x });
		volume.y = std::max({ p0.y, p1.y, p2.y, p3.y });
		volume.z = std::max({ p0.z, p1.z, p2.z, p3.z });
	}*/

	void CullingObject::SetAABBFromTransform(const glm::vec3& center, const glm::vec3& halfVolume, const Transform* transform)
	{
		this->center = transform->GetPosition() + transform->GetDirections() * (center * transform->GetScale());

		glm::vec3 scaledHalfVolume = halfVolume * transform->GetScale();
		this->halfVolume = scaledHalfVolume.x * glm::abs(static_cast<const glm::vec3&>(transform->GetRight()))
			+ scaledHalfVolume.y * glm::abs(static_cast<const glm::vec3&>(transform->GetUp()))
			+ scaledHalfVolume.z * glm::abs(static_cast<const glm::vec3&>(transform->GetForward()));

		this->size = std::max({ this->halfVolume.x, this->halfVolume.y, this->halfVolume.z }) * 2.f;

		this->aab.begin = this->center - this->halfVolume;
		this->aab.end = this->center + this->halfVolume;
	}
}