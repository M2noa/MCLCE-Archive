#pragma once

#include "OctreeCullingAdder.h"

#include "General/Settings.h"
#include "General/Color.h"
#include "../Culling.h"
#include "../CullingCheckers/FrustumCullingChecker.h"

namespace Esteem
{
	// TODO: make this octree independent of the Settings class
	template<Partitioning::OctreeNodeWrapping Wrapping>
	OctreeCullingAdder<Wrapping>::OctreeCullingAdder(Culling* culling, FrustumCullingChecker* frustumChecker)
		: culling(culling)
		, frustumChecker(frustumChecker)
	{ }

	template<Partitioning::OctreeNodeWrapping Wrapping>
	void OctreeCullingAdder<Wrapping>::AddObjects()
	{
		auto& root = Base::root;

		if (root.dirty)
		{
			for(const auto& leaf : root.leaves)
				RegisterCullingObject(leaf);

			root.leaves.clear();

			for (size_t i = 0; i < root.childs.size(); ++i)
			{
				if (auto* child = root.childs[i].get())
					Base::CleanUp(*child, i);
			}

			root.dirty = false;
		}

		for (const auto& child : root.childs)
		{
			if (child)
				CullingPass(*child.get());
		}
	}

	template<Partitioning::OctreeNodeWrapping Wrapping>
	void OctreeCullingAdder<Wrapping>::CullingPass(const Node& partition)
	{
		if (frustumChecker->CubeInFrustum(partition.aab))
		{
			for (const auto& child : partition.childs)
			{
				if (child)
					CullingPass(*child.get());
			}

			for (const auto& leaf : partition.leaves)
			{
				if(frustumChecker->CubeInFrustum(leaf->GetAABB()))
					culling->AppendRenderObjectsDirectly(leaf->GetRenderObjects());
			}
		}
	}

	template<Partitioning::OctreeNodeWrapping Wrapping>
	void OctreeCullingAdder<Wrapping>::ResetSizes(Node* node, const glm::vec3& min, const glm::vec3& max)
	{
		bool isSet = false;

		// Make it nice and tight
		if (min.x < node->aab.begin.x) { node->aab.begin.x = min.x; isSet = true; }
		if (min.y < node->aab.begin.y) { node->aab.begin.y = min.y; isSet = true; }
		if (min.z < node->aab.begin.z) { node->aab.begin.z = min.z; isSet = true; }

		if (max.x > node->aab.end.x) { node->aab.end.x = max.x; isSet = true; }
		if (max.y > node->aab.end.y) { node->aab.end.y = max.y; isSet = true; }
		if (max.z > node->aab.end.z) { node->aab.end.z = max.z; isSet = true; }

		if (node->parent != nullptr && isSet)
			ResetSizes(node->parent, min, max);
	}
	
	template<Partitioning::OctreeNodeWrapping Wrapping>
	void OctreeCullingAdder<Wrapping>::ResetSizes(Node* node, float volumeSize)
	{
		bool isSet = false;

		// Loosely fit
		float overSize = ((node->aab.end.x - node->aab.begin.x) - node->size) * 0.5f;
		if (volumeSize > overSize)
		{
			float deltaSize = volumeSize - overSize;
			node->aab.begin -= deltaSize;
			node->aab.end += deltaSize;
			isSet = true;
		}

		if (node->parent != nullptr && isSet)
			ResetSizes(node->parent, volumeSize);

	}

	template<Partitioning::OctreeNodeWrapping Wrapping>
	void OctreeCullingAdder<Wrapping>::RegisterCullingObject(CullingObject* cullingObject)
	{
		Node* partition = Base::FindOrCreatePartition(Base::root, (glm::ivec3)cullingObject->GetCenter(), (uint)cullingObject->GetSize());
		partition->AddLeaf(cullingObject);

		if constexpr (Wrapping == Partitioning::OctreeNodeWrapping::LOOSE)
			ResetSizes(partition, cullingObject->GetSize());
		else if constexpr (Wrapping == Partitioning::OctreeNodeWrapping::TIGHT)
			ResetSizes(partition, cullingObject->GetAABB().begin, cullingObject->GetAABB().end);
	}

	template<Partitioning::OctreeNodeWrapping Wrapping>
	void OctreeCullingAdder<Wrapping>::UnRegisterCullingObject(CullingObject* cullingObject)
	{
		// TODO: build this
	}

	template<Partitioning::OctreeNodeWrapping Wrapping>
	void OctreeCullingAdder<Wrapping>::DebugRender(std::vector<DevRenderObject>& renderObjects)
	{
		DebugRenderOctree(renderObjects, Base::root, 0);
	}

	template<Partitioning::OctreeNodeWrapping Wrapping>
	void OctreeCullingAdder<Wrapping>::DebugRenderOctree(std::vector<DevRenderObject>& renderObjects, const Node& partition, uint depth)
	{
		depth++;

		for (const auto& child : partition.childs)
		{
			if (child)
			{
				const AxisAlignedBox& aab = child->aab;
				glm::vec3 scale = aab.end - aab.begin;
				glm::vec3 center = aab.begin + scale * 0.5f;

				uint d = (depth - 1) % 5;
				Color8u color;
				switch (d)
				{
				case 0: color = Color8u::red; break;
				case 1:	color = Color8u::blue; break;
				case 2:	color = Color8u::yellow; break;
				case 3: color = Color8u::magenta; break;
				case 4:	color = Color8u::cyan; break;
				}

				renderObjects.emplace_back(DevRenderObject::GeometryType::BOX, DevRenderObject::RenderType::LINES, center, glm::quat(), scale, color);

				for (uint i = 0; i < child->leaves.size(); ++i)
				{
					CullingObject* cullingObject = child->leaves[i];
					glm::vec3 cScale = cullingObject->GetAABB().end - cullingObject->GetAABB().begin;
					glm::vec3 cCenter = cullingObject->GetAABB().begin + cScale * 0.5f;
					renderObjects.emplace_back(DevRenderObject::GeometryType::BOX, DevRenderObject::RenderType::LINES, cCenter, glm::quat(), cScale, Color8u::green);
				}

				DebugRenderOctree(renderObjects, *child.get(), depth);
			}
		}
	}
}