#pragma once

#include "Rendering/Objects/DevRenderObject.h"
#include "Culling/Objects/CullingObject.h"

namespace Esteem
{
	class ICullingAdder
	{
	public:
		virtual ~ICullingAdder() = 0;

		virtual void AddObjects() = 0;

		virtual void RegisterCullingObject(CullingObject* cullingObject) = 0;
		virtual void UnRegisterCullingObject(CullingObject* cullingObject) = 0;

		virtual void DebugRender(std::vector<DevRenderObject>& renderObjects) = 0;
	};

	inline ICullingAdder::~ICullingAdder() {}
}