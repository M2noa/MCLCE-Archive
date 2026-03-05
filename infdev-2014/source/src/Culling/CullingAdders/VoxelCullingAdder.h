#pragma once

#include <mutex>
#include "./ICullingAdder.h"
#include "Memory/Partitioning/Spatial/ISpatialObjectContainer.h"

namespace Esteem
{
	class FrustumCullingChecker;

	class VoxelCullingAdder : public ICullingAdder, public Partitioning::ISpatialObjectContainer
	{
	private: 
		Culling* culling;
		FrustumCullingChecker* frustumChecker;

		std::mutex lock;
		std::vector<CullingObject*> objects;

	public:
		VoxelCullingAdder(Culling* culling, FrustumCullingChecker* frustumChecker);
		~VoxelCullingAdder() = default;

		virtual void AddObjects();

		virtual void RegisterCullingObject(CullingObject* cullingObject);
		virtual void UnRegisterCullingObject(CullingObject* cullingObject);

		virtual void DebugRender(std::vector<DevRenderObject>& renderObjects);

		virtual void Translate(Partitioning::SpatialObject* cullingObject);
		virtual void Scale(Partitioning::SpatialObject* cullingObject);
		virtual void Remove(Partitioning::SpatialObject* cullingObject);
	};
}