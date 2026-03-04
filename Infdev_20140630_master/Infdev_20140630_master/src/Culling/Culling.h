#pragma once

#include <vector>
#include <cppu/stor/lock/deque.h>

#include "./CullingCheckers/ICullingChecker.h"
#include "./CullingAdders/ICullingAdder.h"

#include "./CullingAdders/OctreeCullingAdder.h"
#include "./CullingCheckers/FrustumCullingChecker.h"

#include "Rendering/Objects/DevRenderObject.h"
#include "Rendering/Objects/IRenderData.h"

#include <cppu/cgc/pointers.h>

namespace Esteem
{
	class Light;
	class World;
	class RenderCamera;

	class Culling
	{
	private:
		std::vector<ICullingChecker*> cullingCheckers;
		ICullingAdder* cullingAdders[CullingObject::Type::COUNT];

		RenderList renderList;
		std::vector<Light*> lights;

		// Octree
		FrustumCullingChecker frustumChecker;

		// locking (TODO: need different approach)
		std::mutex lock;

	public:
		Culling();
		~Culling() = default;
		
		void PreRenderUpdate(RenderCamera* renderCamera);

		RenderList& GetRenderObjectList();
		std::vector<Light*>& GetLightList();

		void DebugRender(std::vector<DevRenderObject>& renderObjects);

		/// \brief Add or remove a world object into the culling system
		/// \param worldObject the object that will be added
		/// \param add if true the object will be added, if false it will be removed (used for delegate)
		bool RegisterCullingObject(CullingObject* cullingObject);
		static void UnRegisterCullingObject(CullingObject* cullingObject);

		void AddObjectToRenderList(RenderObject* renderObject);

		void AppendRenderObjectsDirectly(const std::vector<cgc::strong_ptr<RenderObject>>& renderObjects);
		void AppendRenderObjectsDirectly(const stor::lock::deque<cgc::strong_ptr<RenderObject>>& renderObjects);

		void AppendRenderObjectsDirectly(const std::vector<RenderObject*>& renderObjects);
		void AppendRenderObjectsDirectly(const stor::lock::deque<RenderObject*>& renderObjects);

		inline std::mutex& GetLock() { return lock; }
	};
}

#include "./Culling.inl"