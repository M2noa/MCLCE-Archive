#include "OpenGLRenderer.h"

#include <iostream>

#include "Utils/Debug.h"

#include "./OpenGLDebug.h"

#include "./Techniques/ForwardRendering.h"
#include "./Techniques/LightIndexedRendering.h"
#include "./Techniques/DeferredRendering.h"
#include "./Techniques/OverlayRendering.h"

#include "Window/View.h"
#include "Client/RenderView.h"
#include "General/Settings.h"
#include "World/World.h"

#include "OpenGLFactory.h"

#define M_PI 3.141592653589793238462643383279502884

namespace Esteem
{
	namespace OpenGL
	{
		OpenGLRenderer::OpenGLRenderer(glm::uvec2 screenSize)
			: Renderer(screenSize)
		{
			
		}

		void OpenGLRenderer::ThreadInitialize()
		{
			// Initialize Glew
			glewExperimental = GL_TRUE;
			GLenum glewError = glewInit();
			if (glewError != GLEW_OK)
				Debug::LogError(std::string(reinterpret_cast<const char*>(glewGetErrorString(glewError))));
			
			LogOpenGLErrors();

			//glShadeModel(GL_SMOOTH);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClearDepth(1.0f);

			// initialize ubo and store light data
			int lightAmount = 256;
			lightsUBO = cgc::static_pointer_cast<UBO>(factory.LoadUBO(nullptr, lightAmount * sizeof(LightData), "lightsUB"));

			// initialize camera ubo
			cameraUBO = cgc::static_pointer_cast<UBO>(factory.LoadUBO(nullptr, sizeof(RenderCameraData), "cameraUB"));
			
			Debug::Log(" -------------------------------------------------------------------------------------");

			// Setup main Rendering Technique
			switch (Settings::renderMode)
			{
			case Settings::RenderMode::FORWARD_RENDERING:
				renderTechnique = new ForwardRendering(*this);
				factory.SetShaderDefinition("RENDERMODE_FORWARD", "");
				Debug::Log("| RENDERING MODE: \t\t\tForwardRendering");
				break;
			case Settings::RenderMode::DEFERRED_RENDERING:
				renderTechnique = new DeferredRendering(*this);
				factory.SetShaderDefinition("RENDERMODE_DEFERRED", "");
				Debug::Log("| RENDERING MODE: \t\t\tDeferredRendering");
				break;
			case Settings::RenderMode::LIGHT_INDEXED_RENDERING:
				renderTechnique = new LightIndexedRendering(*this);
				factory.SetShaderDefinition("RENDERMODE_FORWARD", "");
				factory.SetShaderDefinition("RENDERMODE_LIGHT_INDEXED", "");
				Debug::Log("| RENDERING MODE: \t\t\tLightIndexedRendering");
				break;
			}

			// Setup overlay technique
			overlayRenderTechnique = new OverlayRendering(*this);

			//Debug::Log("| SHADOW ATLAS SIZE: \t\t\t" + std::to_string(Settings::shadowAtlasSize.x) + "x" + std::to_string(Settings::shadowAtlasSize.y));
			//Debug::Log("| CASCADED MAP SIZE: \t\t\t" + std::to_string(Settings::cascadedShadowMapSize.x) + "x" + std::to_string(Settings::cascadedShadowMapSize.y));
			//Debug::Log("|");

			int val;
			int val2;

			glGetIntegerv(GL_MAJOR_VERSION, &val);
			glGetIntegerv(GL_MINOR_VERSION, &val2);
			Debug::Log("| GL_VERSION: \t\t\t\t" + std::to_string(val) + "." + std::to_string(val2));

			glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &val);
			Debug::Log("| GL_MAX_UNIFORM_BUFFER_BINDINGS: \t" + std::to_string(val));

			glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &val);
			Debug::Log("| GL_MAX_UNIFORM_BLOCK_SIZE: \t\t" + std::to_string(val));

			glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &val);
			Debug::Log("| GL_MAX_TEXTURE_IMAGE_UNITS: \t\t" + std::to_string(val));

			Debug::Log(" -------------------------------------------------------------------------------------");

			// initialize timing
			queryID = 0;
			glGenQueries(2, &query[0]);
			glBeginQuery(GL_TIME_ELAPSED, query[0]);
			glEndQuery(GL_TIME_ELAPSED);
			glBeginQuery(GL_TIME_ELAPSED, query[1]);
			glEndQuery(GL_TIME_ELAPSED);

			//set default values
			//glEnable(GL_PRIMITIVE_RESTART);
			//glPrimitiveRestartIndex(0);

			// Debug/Dev Rendering
			devRenderObjectsShader = factory.LoadOpenGLShader("DevRenderObjects");

			// Load in queued data
			factory.HandleQueue();
		}

		void OpenGLRenderer::RenderFrame()
		{
			std::unique_lock<std::mutex> lock = factory.GetLock();
			glBeginQuery(GL_TIME_ELAPSED, query[queryID]);
			
			OpenGLRenderData& renderData = *static_cast<OpenGLRenderData*>(world->GetRenderData());
			cameraUBO->UpdateBuffer(&renderData.GetCamera()->GetRenderCameraData()->data, sizeof(RenderCameraData));
			
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			glDepthFunc(GL_LEQUAL);

			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT); // Cull back-facing triangles -> draw only front-facing triangles
			glDisable(GL_BLEND);


			glViewport(0, 0, screenSize.x, screenSize.y); // Set our viewport to the size of our window
			//glClearColor(0, 0, 0, 1);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			renderTechnique->RenderFrame(renderData);
			overlayRenderTechnique->RenderFrame(renderData);

			glEndQuery(GL_TIME_ELAPSED);

			queryID = queryID ^ 1; // == 1 ? 0 : 1;
			glGetQueryObjectuiv(query[queryID], GL_QUERY_RESULT, &elapsedTime);

			//LogOpenGLErrors();
		}
		
		void OpenGLRenderer::RenderDevObjects(const std::vector<DevRenderObject>& renderObjects)
		{
			FBO::UnBind(*this);
			devRenderObjectsShader->Bind();
			for (uint i = 0; i < renderObjects.size(); ++i)
			{
				const DevRenderObject& renderObject = renderObjects[i];
				glm::vec3 begin = renderObject.position - (renderObject.scale * 0.5f);
				glm::vec3 end = renderObject.position + (renderObject.scale * 0.5f);

				glColor3ub(renderObject.color.r, renderObject.color.g, renderObject.color.b);
				glBegin(GL_LINES);
				{
					// top
					glVertex3f(begin.x, begin.y, begin.z);
					glVertex3f(end.x, begin.y, begin.z);

					glVertex3f(end.x, begin.y, begin.z);
					glVertex3f(end.x, begin.y, end.z);

					glVertex3f(begin.x, begin.y, end.z);
					glVertex3f(end.x, begin.y, end.z);

					glVertex3f(begin.x, begin.y, begin.z);
					glVertex3f(begin.x, begin.y, end.z);


					// middle
					glVertex3f(begin.x, begin.y, begin.z);
					glVertex3f(begin.x, end.y, begin.z);

					glVertex3f(end.x, begin.y, begin.z);
					glVertex3f(end.x, end.y, begin.z);

					glVertex3f(begin.x, end.y, end.z);
					glVertex3f(begin.x, begin.y, end.z);

					glVertex3f(end.x, end.y, end.z);
					glVertex3f(end.x, begin.y, end.z);

					// bottom
					glVertex3f(begin.x, end.y, begin.z);
					glVertex3f(end.x, end.y, begin.z);

					glVertex3f(end.x, end.y, begin.z);
					glVertex3f(end.x, end.y, end.z);

					glVertex3f(begin.x, end.y, end.z);
					glVertex3f(end.x, end.y, end.z);

					glVertex3f(begin.x, end.y, begin.z);
					glVertex3f(begin.x, end.y, end.z);
				}
				glEnd();
			}
			OpenGLShader::UnBind();
		}

		uint OpenGLRenderer::GetRenderTime()
		{
			return ((uint)elapsedTime / 1000000);
		}

		void OpenGLRenderer::ActivateTexture(uint textureIndex, std::size_t uniformVariable, Texture2D* texture, cgc::raw_ptr<OpenGLShader> shader)
		{
			auto variable = shader->GetUniformVariable(uniformVariable);
			if(variable != nullptr)
			{
				glActiveTexture(GL_TEXTURE0 + textureIndex);
				glBindTexture(GL_TEXTURE_2D, texture->GetID());
				glUniform1i(variable->index, textureIndex); // TODO make sure the correct shader is bound
				enabledTextures[textureIndex] = texture->GetID();
			}
		}

		void OpenGLRenderer::ActivateTexture(uint textureIndex, std::size_t uniformVariable, Texture3D* texture, cgc::raw_ptr<OpenGLShader> shader)
		{
			auto variable = shader->GetUniformVariable(uniformVariable);
			if(variable != nullptr)
			{
				glActiveTexture(GL_TEXTURE0 + textureIndex);
				glBindTexture(GL_TEXTURE_2D, texture->GetID());
				glUniform1i(variable->index, textureIndex); // TODO make sure the correct shader is bound
				enabledTextures[textureIndex] = texture->GetID();
			}
		}
		
		void OpenGLRenderer::ActivateTexture(uint textureIndex, std::size_t uniformVariable, TextureCube* texture, cgc::raw_ptr<OpenGLShader> shader)
		{
			auto variable = shader->GetUniformVariable(uniformVariable);
			if(variable != nullptr)
			{
				glActiveTexture(GL_TEXTURE0 + textureIndex);
				glBindTexture(GL_TEXTURE_CUBE_MAP, texture->GetID());
				glUniform1i(variable->index, textureIndex); // TODO make sure the correct shader is bound
				enabledTextures[textureIndex] = texture->GetID();
			}
		}
		
		void OpenGLRenderer::ReInitialize()
		{
			factory.ReInitialize();
		}

		void OpenGLRenderer::SetWorld(World* world)
		{
			this->world = world;
			if (!world->GetRenderData())
				world->SetRenderData(std::make_unique<OpenGLRenderData>());
		}

		void OpenGLRenderer::SetThreadID(const std::thread::id& id)
		{
			factory.SetThreadID(id);
		}

		void OpenGLRenderer::ClearActiveTextures()
		{
			for(uint i = 0; i < 32; i++)
				enabledTextures[i] = 65535;
		}

		OpenGLRenderer::~OpenGLRenderer()
		{
			delete overlayRenderTechnique;
			delete renderTechnique;
		}
	}
}