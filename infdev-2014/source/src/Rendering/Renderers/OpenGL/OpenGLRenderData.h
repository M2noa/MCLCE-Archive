#pragma once

#include <vector>
#include <array>

#include "../../Objects/IRenderData.h"
#include "../../Objects/RenderObject.h"
#include "./Objects/OpenGLRenderObject.h"
#include "Rendering/Objects/RenderCamera.h"
#include "Rendering/Objects/LightObject.h"

#include <cppu/cgc/pointers.h>

namespace Esteem
{
	namespace OpenGL
	{
		typedef std::array<std::vector<OpenGLRenderObject*>, RenderObject::RenderOrder::COUNT> OpenGLRenderList;

		class OpenGLRenderData : public IRenderData
		{
		private:
			OpenGLRenderList renderList;
			const std::vector<LightRenderData>* lights;
			cgc::strong_ptr<Camera> renderCamera;

		public:
			virtual RenderList& GetAbstractRenderList();
			const OpenGLRenderList& GetRenderList() const;

			virtual const std::vector<LightRenderData>* GetLights() const;
			virtual const cgc::strong_ptr<Camera>& GetCamera() const;

			virtual void SetRenderList(const RenderList& renderList);
			virtual void SetLights(const std::vector<LightRenderData>* lights);
			virtual void SetRenderCamera(const cgc::strong_ptr<Camera>& renderCamera);
		};
	}
}

#include "./OpenGLRenderData.inl"