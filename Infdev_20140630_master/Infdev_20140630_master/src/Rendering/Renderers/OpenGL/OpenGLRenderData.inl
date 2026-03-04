#pragma once

#include "./OpenGLRenderData.h"

namespace Esteem
{
	namespace OpenGL
	{
		inline RenderList& OpenGLRenderData::GetAbstractRenderList()
		{
			// NOTE: safe?
			return reinterpret_cast<RenderList&>(renderList);
		}

		inline const OpenGLRenderList& OpenGLRenderData::GetRenderList() const
		{
			return renderList;
		}

		inline const std::vector<LightRenderData>* OpenGLRenderData::GetLights() const
		{
			return lights;
		}
		
		inline const cgc::strong_ptr<Camera>& OpenGLRenderData::GetCamera() const
		{
			return renderCamera;
		}

		inline void OpenGLRenderData::SetRenderList(const RenderList& renderList)
		{
			this->renderList = (OpenGLRenderList&)renderList;
		}

		inline void OpenGLRenderData::SetLights(const std::vector<LightRenderData>* lights)
		{
			this->lights = lights;
		}

		inline void OpenGLRenderData::SetRenderCamera(const cgc::strong_ptr<Camera>& renderCamera)
		{
			this->renderCamera = renderCamera;
		}
	}
}