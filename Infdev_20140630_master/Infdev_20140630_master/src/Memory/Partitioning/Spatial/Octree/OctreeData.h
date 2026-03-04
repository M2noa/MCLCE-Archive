#pragma once

#include <array>
#include <cppu/stor/lock/deque.h>
#include "stdafx.h"

#include "../AxisAlignedBox.h"
#include "../SpatialObject.h"
#include "../ISpatialObjectContainer.h"

namespace Esteem
{
	namespace Partitioning
	{
		enum class OctreeNodeWrapping
		{
			DEFAULT,
			LOOSE,
			TIGHT,
		};

		template<class L>
		struct OctreeNode;

		class CullingObject;

		template<class L, size_t, size_t, size_t, size_t, OctreeNodeWrapping>
		class Octree;

		template<class L>
		struct OctreeNode : ISpatialObjectContainer
		{
			template<class L, size_t, size_t, size_t, size_t, OctreeNodeWrapping>
			friend class Octree;

		public:
			bool dirty;
			std::shared_mutex lock;

			uint32_t size;
			glm::ivec3 center;

			AxisAlignedBox aab;

			OctreeNode<L>* parent;
			std::array<std::unique_ptr<OctreeNode<L>>, 8> childs;
			std::vector<L*> leaves;

			// disable copy
			OctreeNode(const OctreeNode<L>&) = delete;
			void operator=(const OctreeNode<L>&) = delete;

			OctreeNode(const AxisAlignedBox& aab, OctreeNode<L>* parent, const glm::ivec3& center, uint32_t size)
				: aab(aab), parent(parent), size(size), dirty(false), center(center)
			{}

			OctreeNode()
				: center(glm::ivec3(0, 0, 0))
			{}

			inline void AddLeaf(L* leaf)
			{
				leaves.push_back(leaf);
				leaf->SetContainer(this);
			}

			inline void RemoveLeaf(L* leaf)
			{
				auto found = std::find(leaves.begin(), leaves.end(), leaf);
				if(found != leaves.end())
					RemoveLeafByIterator(found);
			}

			inline void RemoveLeafAt(size_t index)
			{
				RemoveLeafByIterator(leaves.begin() + index);
			}

			void SetDirty()
			{
				if (!this->dirty)
				{
					this->dirty = true;

					if (this->parent != nullptr)
						this->parent->SetDirty();
				}
			}

			void PlaceObjectInRoot(L* leaf)
			{
				if (this->parent != nullptr)
					this->parent->PlaceObjectInRoot(leaf);
				else
					this->leaves.push_back(leaf);
			}

			virtual void Translate(SpatialObject* cullingObject)
			{
				const glm::vec3& min = cullingObject->GetAABB().begin;
				const glm::vec3& max = cullingObject->GetAABB().end;

				if (min.x < aab.begin.x || min.y < aab.begin.y || min.z < aab.begin.z ||
					max.x > aab.end.x || max.y > aab.end.y || max.z > aab.end.z)
				{
					auto found = std::find(leaves.begin(), leaves.end(), cullingObject);
					if (found != leaves.end())
						leaves.erase(found);

					PlaceObjectInRoot(static_cast<L*>(cullingObject));

					SetDirty();
				}
			}

			virtual void Scale(SpatialObject* cullingObject) {}

			virtual void Remove(SpatialObject* cullingObject)
			{
				auto found = std::find(leaves.begin(), leaves.end(), cullingObject);
				if (found != leaves.end())
				{
					leaves.erase(found);
					SetDirty();
				}
			}

		protected:
			inline virtual void RemoveLeafByIterator(const typename std::vector<L*>::iterator& it)
			{
				L* leaf = *it;
				leaves.erase(it);
			}
		};
	}
}