#pragma once

#include <map>
#include <vector>

#include "./AxisAlignedBox.h"
#include "./ISpatialObjectContainer.h"

namespace Esteem
{
	namespace Partitioning
	{
		struct SpatialObject
		{
		protected:
			glm::vec3 center;
			glm::vec3 halfVolume;
			float size;
			AxisAlignedBox aab;

			ISpatialObjectContainer* container;

		public:
			SpatialObject() = default;
			SpatialObject(const glm::vec3& center);

			void SetCenter(const glm::vec3& center);
			const glm::vec3& GetCenter() const;

			void SetHalfVolume(const glm::vec3& halfVolume);
			void SetAABB(const glm::vec3& begin, const glm::vec3& end);

			const AxisAlignedBox& GetAABB() const;
			void RecalculateAABB();

			float GetSize() const;

			ISpatialObjectContainer* GetContainer()  const;
			void SetContainer(ISpatialObjectContainer* container);

			void RemoveFromCulling();

			~SpatialObject();

			/*void SetVolumeBoxSize(glm::vec3 size) { this->halfVolume = size * 0.5f; }
			inline const glm::vec3 GetVolumeBoxSize() const	{ return this->halfVolume * 2.f; }

			void SetHalfVolume(glm::vec3 size) { this->halfVolume = size; }
			inline const glm::vec3 GetHalfVolume() const { return this->halfVolume; }*/

			//void SetAABBFromOBB();
		};
	}
}

#include "./SpatialObject.inl"