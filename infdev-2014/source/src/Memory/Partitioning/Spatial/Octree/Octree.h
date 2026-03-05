#pragma once

#include "stdafx.h"
#include <vector>
#include <list>
#include <glm/vec3.hpp>

#include "./OctreeData.h"

namespace Esteem
{
	namespace Partitioning
	{
		template<class L, size_t MaxGridSize, size_t MinGridSize, size_t DivideCount, size_t MergeCount, OctreeNodeWrapping Wrapping = OctreeNodeWrapping::DEFAULT>
		class Octree
		{
		public:
			typedef L Leaf;
			typedef OctreeNode<L> Node;

		protected:
			Node root;

			static AxisAlignedBox CreateAxisAlignedBox(const glm::ivec3& position, uint size);

			static void DivideBox(Node& node);

			static void MergeBox(Node& node);

			static bool CleanUp(Node& node, size_t parentIndex);

			static uint8 OctreeChildIndex(const Node& node, const glm::ivec3& position);

			//template<typename T = AddRemoveClass, typename = std::enable_if_t<std::is_same<NoAddRemoveClass, T>::value, T>>
			//static void MergeBox(Node* node);

			static uint GetObjectsCountRecursive(const Node& node);
			static uint GetSmallObjectsCount(const Node& node);

		public:
			// disable copy
			Octree(const Octree&) = delete;
			void operator=(const Octree&) = delete;

			Octree();

			/*typename std::enable_if<std::is_pointer<L>::value>::type
			virtual void QueryObjects(const AxisAlignedBox& aabb, std::vector<L>& list);

			typename std::enable_if<!std::is_pointer<L>::value>::type
			virtual void QueryObjects(const AxisAlignedBox& aabb, std::vector<L*>& list);*/

			void AddLeaf(L* leaf, const glm::ivec3& position, uint minSize);
			void RemoveLeaf(L* leaf);

			static Node* FindOrCreatePartition(Node& root, const glm::ivec3& position, uint minSize);
			static Node* FindPartition(Node& root, const glm::ivec3& position);
			static Node* CreatePartition(Node& parent, const glm::ivec3& position, uint size, uint8 cellIndex);
		};
	}
}

#include "Octree.inl"