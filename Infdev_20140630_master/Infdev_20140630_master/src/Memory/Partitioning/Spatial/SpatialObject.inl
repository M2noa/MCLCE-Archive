#pragma once

#include "SpatialObject.h"

namespace Esteem
{
	namespace Partitioning
	{
		inline SpatialObject::SpatialObject(const glm::vec3& center)
			: center(center)
			, halfVolume()
			, size()
			, aab()
			, container(nullptr)
		{}

		inline void SpatialObject::SetCenter(const glm::vec3& center)
		{
			this->center = center;
		}

		inline const glm::vec3& SpatialObject::GetCenter() const
		{
			return this->center;
		}

		inline void SpatialObject::SetHalfVolume(const glm::vec3& halfVolume)
		{
			this->halfVolume = halfVolume;
		}

		inline void SpatialObject::SetAABB(const glm::vec3& begin, const glm::vec3& end)
		{
			aab.begin = begin; aab.end = end;
		};

		inline const AxisAlignedBox& SpatialObject::GetAABB() const
		{
			return this->aab;
		}

		inline float SpatialObject::GetSize() const
		{
			return size;
		}

		inline void SpatialObject::RecalculateAABB()
		{
			aab.begin = center - halfVolume; aab.end = center + halfVolume;
		}

		inline ISpatialObjectContainer* SpatialObject::GetContainer() const
		{
			return this->container;
		}

		inline void SpatialObject::SetContainer(ISpatialObjectContainer* container)
		{
			if (this->container)
				this->container->Remove(this);

			this->container = container;
		}

		inline void SpatialObject::RemoveFromCulling()
		{
			if (this->container)
			{
				this->container->Remove(this);
				this->container = nullptr;
			}
		}

		inline SpatialObject::~SpatialObject()
		{
			if (this->container)
				this->container->Remove(this);
		}
	}
}