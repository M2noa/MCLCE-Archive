#include "Octree.h"

namespace Esteem
{
	namespace Partitioning
	{
		template<class L, size_t MaxGridSize, size_t MinGridSize, size_t DivideCount, size_t MergeCount, OctreeNodeWrapping Wrapping>
		Octree<L, MaxGridSize, MinGridSize, DivideCount, MergeCount, Wrapping>::Octree()
			: root(AxisAlignedBox(glm::vec3(0, 0, 0), float(MaxGridSize), false), nullptr, glm::ivec3(0, 0, 0), MaxGridSize)
		{
			static_assert(MergeCount <= DivideCount, "Octree::Octree(): MergeCount can not be bigger than DivideCount");
			static_assert(MinGridSize <= MaxGridSize, "Octree::Octree(): MinGridSize can not be bigger than MaxGridSize");
		}

		template<class L, size_t MaxGridSize, size_t MinGridSize, size_t DivideCount, size_t MergeCount, OctreeNodeWrapping Wrapping>
		bool Octree<L, MaxGridSize, MinGridSize, DivideCount, MergeCount, Wrapping>::CleanUp(Node& node, size_t parentIndex)
		{
			if (node.dirty)
			{
				size_t childCount = 0;
				for (size_t i = 0; i < node.childs.size(); ++i)
				{
					if (Node* child = node.childs[i].get())
					{
						++childCount;
						CleanUp(*child, i);
					}
				}

				if (GetObjectsCountRecursive(node) <= MergeCount)
					MergeBox(node);

				if (node.parent != nullptr && childCount == 0 && node.leaves.empty())
					node.parent->childs[parentIndex] = nullptr;
				else
					node.dirty = false;
			}

			return false;
		}

		template<class L, size_t MaxGridSize, size_t MinGridSize, size_t DivideCount, size_t MergeCount, OctreeNodeWrapping Wrapping>
		void Octree<L, MaxGridSize, MinGridSize, DivideCount, MergeCount, Wrapping>::AddLeaf(L* leaf, const glm::ivec3& position, uint minSize)
		{
			uint8_t index = OctreeChildIndex(root, position);
			auto* child = root.childs[index].get();
			Node* partition = child ? FindOrCreatePartition(*child, position, minSize) : CreatePartition(root, position, minSize, index);
			partition->AddLeaf(leaf);
		}

		template<class L, size_t MaxGridSize, size_t MinGridSize, size_t DivideCount, size_t MergeCount, OctreeNodeWrapping Wrapping>
		inline void Octree<L, MaxGridSize, MinGridSize, DivideCount, MergeCount, Wrapping>::RemoveLeaf(L * leaf)
		{
			// TODO: build this
		}

		template<class L, size_t MaxGridSize, size_t MinGridSize, size_t DivideCount, size_t MergeCount, OctreeNodeWrapping Wrapping>
		void Octree<L, MaxGridSize, MinGridSize, DivideCount, MergeCount, Wrapping>::MergeBox(Node& partition)
		{
			for (auto& child : partition.childs)
			{
				if (child)
				{
					MergeBox(*child);

					partition.leaves.reserve(partition.leaves.size() + child->leaves.size());
					for (auto& leaf : child->leaves)
					{
						leaf->SetContainer(&partition);
						partition.leaves.push_back(leaf);
					}

					child = nullptr;
				}
			}
		}

		template<class L, size_t MaxGridSize, size_t MinGridSize, size_t DivideCount, size_t MergeCount, OctreeNodeWrapping Wrapping>
		void Octree<L, MaxGridSize, MinGridSize, DivideCount, MergeCount, Wrapping>::DivideBox(Node& partition)
		{
			uint halfSize = partition.size / 2;
			for (size_t i = partition.leaves.size(); i-- >= 0;)
			{
				L* leaf = partition.leaves[i];
				if (leaf->GetSize() <= halfSize)
				{
					partition.RemoveLeafAt(i);

					glm::ivec3 position = (glm::ivec3)leaf->GetCenter();

					uint index = OctreeChildIndex(partition, position);
					Node* child = partition.childs[index].get();
					if (child == nullptr)
						child = CreatePartition(partition, position, halfSize, index);

					child->AddLeaf(leaf);
				}
			}
		}

		template<class L, size_t MaxGridSize, size_t MinGridSize, size_t DivideCount, size_t MergeCount, OctreeNodeWrapping Wrapping>
		auto Octree<L, MaxGridSize, MinGridSize, DivideCount, MergeCount, Wrapping>::FindOrCreatePartition(Node& root, const glm::ivec3& position, uint minSize) -> Node*
		{
			uint32_t halfSize = root.size / 2;

			if (root.size <= MinGridSize || minSize > halfSize)
				return &root;
			
			if constexpr (MergeCount != DivideCount)
			{
				if (GetSmallObjectsCount(root) + 1 >= DivideCount)
					DivideBox(root);
			}

			uint8_t index = OctreeChildIndex(root, position);
			auto* child = root.childs[index].get();
			return child
				? FindOrCreatePartition(*child, position, minSize)
				: CreatePartition(root, position, halfSize, index);
		}

		template<class L, size_t MaxGridSize, size_t MinGridSize, size_t DivideCount, size_t MergeCount, OctreeNodeWrapping Wrapping>
		auto Octree<L, MaxGridSize, MinGridSize, DivideCount, MergeCount, Wrapping>::FindPartition(Node& node, const glm::ivec3& position) -> Node*
		{
			auto* child = node.childs[OctreeChildIndex(node, position)].get();
			return child ? FindPartition(*child, position) : &node;
		}

		template<class L, size_t MaxGridSize, size_t MinGridSize, size_t DivideCount, size_t MergeCount, OctreeNodeWrapping Wrapping>
		auto Octree<L, MaxGridSize, MinGridSize, DivideCount, MergeCount, Wrapping>::CreatePartition(Node& parent, const glm::ivec3& position, uint size, uint8 cellIndex) -> Node*
		{
			// create the aab
			glm::ivec3 center = parent.center;
			int halfSize = size / 2;

			center.x += -halfSize + (((cellIndex >> 2) & 0x1) * size);
			center.y += -halfSize + (((cellIndex >> 1) & 0x1) * size);
			center.z += -halfSize + (((cellIndex) & 0x1) * size);
						
			Node* partition = (parent.childs[cellIndex] = std::make_unique<Node>(CreateAxisAlignedBox(center, size), &parent, center, size)).get();

			return partition;

#ifdef false //OCTREE_MERGE_AND_DIVIDE
			// is this the right aab?
			if (MinSize >= partition->size / 2)
				return partition;

			uint8 aabIndex = OctreeChildIndex(partition, position);

			// not the right aab keep on creating
			if (halfSize >= MinGridSize)
				partition = CreatePartition(partition, position, minSize, aabIndex);

			return partition;
#endif
		}

		template<class L, size_t MaxGridSize, size_t MinGridSize, size_t DivideCount, size_t MergeCount, OctreeNodeWrapping Wrapping>
		inline AxisAlignedBox Octree<L, MaxGridSize, MinGridSize, DivideCount, MergeCount, Wrapping>::CreateAxisAlignedBox(const glm::ivec3& position, uint size)
		{
			if constexpr (Wrapping == Partitioning::OctreeNodeWrapping::TIGHT)
				return AxisAlignedBox((glm::vec3)(position + (int)size), (glm::vec3)(position - (int)size));
			else
				return AxisAlignedBox(position, (float)size, false);
		}

		template<class L, size_t MaxGridSize, size_t MinGridSize, size_t DivideCount, size_t MergeCount, OctreeNodeWrapping Wrapping>
		uint Octree<L, MaxGridSize, MinGridSize, DivideCount, MergeCount, Wrapping>::GetObjectsCountRecursive(const Node& partition)
		{
			uint count = partition.leaves.size();

			for (const auto& child : partition.childs)
			{
				if (child)
					count += GetObjectsCountRecursive(*child);
			}

			return count;
		}

		template<class L, size_t MaxGridSize, size_t MinGridSize, size_t DivideCount, size_t MergeCount, OctreeNodeWrapping Wrapping>
		uint Octree<L, MaxGridSize, MinGridSize, DivideCount, MergeCount, Wrapping>::GetSmallObjectsCount(const Node& partition)
		{
			uint count = 0;
			uint halfSize = (uint)(partition.size / 2.f);
			std::unique_lock<std::mutex> lock;

			for (const auto* leaf : partition.leaves)
			{
				if (leaf->GetSize() <= halfSize)
					++count;
			}

			return count;
		}

		template<class L, size_t MaxGridSize, size_t MinGridSize, size_t DivideCount, size_t MergeCount, OctreeNodeWrapping Wrapping>
		inline uint8 Octree<L, MaxGridSize, MinGridSize, DivideCount, MergeCount, Wrapping>::OctreeChildIndex(const Node& partition, const glm::ivec3 & position)
		{
			uint8 index = 0;
			if (position.x >= partition.center.x) index |= 4u;
			if (position.y >= partition.center.y) index |= 2u;
			if (position.z >= partition.center.z) index |= 1u;

			return index;
		}
	}
}