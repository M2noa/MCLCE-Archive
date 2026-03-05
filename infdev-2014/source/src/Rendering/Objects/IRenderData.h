#pragma once

#include <array>
#include <vector>

#include "Rendering/Objects/RenderObject.h"

#include <cppu/cgc/pointers.h>

namespace Esteem
{
	class Light;
	class Camera;

	typedef std::array<std::vector<RenderObject*>, RenderObject::RenderOrder::COUNT> RenderList;

	class IRenderData
	{
	public:
		virtual RenderList& GetAbstractRenderList() = 0;
		virtual const std::vector<LightRenderData>* GetLights() const = 0;
		virtual const cgc::strong_ptr<Camera>& GetCamera() const = 0;

		virtual void SetRenderList(const RenderList& renderList) = 0;
		virtual void SetLights(const std::vector<LightRenderData>* lights) = 0;
		virtual void SetRenderCamera(const cgc::strong_ptr<Camera>& renderCamera) = 0;
	};
}