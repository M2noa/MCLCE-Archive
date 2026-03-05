#pragma once

#include "Memory/Partitioning/Spatial/Octree/Octree.h"
#include "./ICullingAdder.h"

namespace Esteem
{
	class Culling;
	class FrustumCullingChecker;

	template<Partitioning::OctreeNodeWrapping Wrapping = Partitioning::OctreeNodeWrapping::DEFAULT>
	class OctreeCullingAdder : public Partitioning::Octree<CullingObject,
		Settings::CullingOctreeMaxGridSize, Settings::CullingOctreeMinGridSize,
		Settings::CullingOctreeDivideCount, Settings::CullingOctreeMergeCount,
		Wrapping>, public ICullingAdder
	{
	friend class Culling;
	protected:
		typedef typename Partitioning::Octree<CullingObject,
			Settings::CullingOctreeMaxGridSize, Settings::CullingOctreeMinGridSize,
			Settings::CullingOctreeDivideCount, Settings::CullingOctreeMergeCount,
			Wrapping> Base;

		typedef typename Partitioning::Octree<CullingObject,
			Settings::CullingOctreeMaxGridSize, Settings::CullingOctreeMinGridSize,
			Settings::CullingOctreeDivideCount, Settings::CullingOctreeMergeCount,
			Wrapping>::Node Node;

		Culling* culling;
		FrustumCullingChecker* frustumChecker;

		void CullingPass(const Node& partition);

		static void DebugRenderOctree(std::vector<DevRenderObject>& renderObjects, const Node& partition, uint depth);

		static void ResetSizes(Node* node, const glm::vec3& min, const glm::vec3& max);
		static void ResetSizes(Node* node, float size);

	public:
		// disable copy
		OctreeCullingAdder(const OctreeCullingAdder&) = delete;
		void operator=(const OctreeCullingAdder&) = delete;

		OctreeCullingAdder(Culling* culling, FrustumCullingChecker* frustumChecker);
		virtual void AddObjects();

		void RegisterCullingObject(CullingObject* cullingObject);
		virtual void UnRegisterCullingObject(CullingObject* cullingObject);

		virtual void DebugRender(std::vector<DevRenderObject>& renderObjects);

		virtual ~OctreeCullingAdder() = default;
	};
}

#include "OctreeCullingAdder.inl"