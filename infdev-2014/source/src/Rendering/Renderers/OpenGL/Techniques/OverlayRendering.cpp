#include "OverlayRendering.h"

#include "General/Settings.h"
#include "Utils/Diagnostics.h"
#include "../OpenGLFactory.h"

#include "../OpenGLRenderCalls.h"
#include "Rendering/Objects/GraphicalOverlay.h"
#include "World/Constituents/Camera.h"

namespace Esteem
{
	namespace OpenGL
	{
		OverlayRendering::OverlayRendering(OpenGLRenderer& renderer)
			: RenderTechnique(renderer)
		{
			Initialize();
		}

		void OverlayRendering::Initialize()
		{

		}

		void OverlayRendering::RenderFrame(const OpenGLRenderData& renderData)
		{
			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			glDisable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			auto& renderCamera = renderData.GetCamera()->GetRenderCameraData();
			std::vector<GraphicalOverlay*>& graphicOverlays = renderCamera->graphicalOverlays;

			std::size_t graphicOverlaysSize = graphicOverlays.size();
			for (std::size_t overlayIndex = 0; overlayIndex < graphicOverlaysSize; ++overlayIndex)
			{
				GraphicalOverlay* overlay = graphicOverlays[overlayIndex];
				if (overlay->IsVisible())
				{
					const std::vector<RenderObject*>& renderList = overlay->GetRenderList();
					std::size_t renderListSize = renderList.size();
					for (std::size_t i = 0; i < renderListSize; ++i)
					{
						OpenGLRenderObject* renderObject = static_cast<OpenGLRenderObject*>(renderList[i]);
						cgc::raw_ptr<OpenGLMaterial> material = static_cast<OpenGLMaterial*>(renderObject->GetMaterial().ptr());
						cgc::raw_ptr<OpenGLShader> shader = static_cast<OpenGLShader*>(material->GetOpenGLShader().ptr());
						cgc::raw_ptr<VAO> vao = renderObject->GetVAO();

						shader->Bind();
						const glm::mat4* modelMatrix = renderObject->GetModelMatrix();
						if(modelMatrix)
							shader->SetUniform(CT_HASH("ModelMatrix"), *modelMatrix);

						vao->Bind();
						material->Bind();

						DrawRenderObject(*renderObject);

						//LogOpenGLErrors();
						Diagnostics::drawCalls++;
					}
				}
			}
			VAO::UnBind();

			renderCamera->guiContext->Render();

			glDisable(GL_BLEND);
			glEnable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
			
			//OpenGLShader::UnBind();
		}

		OverlayRendering::~OverlayRendering()
		{
		}
	}
}