#pragma once

#include "../../Renderer.h"

#include <thread>
#include <SFML/Graphics.hpp>
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

#include "./Objects/OpenGLRenderObject.h"
#include "../../IRenderTechnique.h"

#include "./OpenGLRenderData.h"
#include "./OpenGLFactory.h"

namespace Esteem
{
	namespace OpenGL
	{
		class OpenGLShader;

		/// \brief OpenGL Renderer
		///  
		/// \author Y. Schaeffer
		/// \date March 2015
		class OpenGLRenderer : public Renderer
		{
		private:
			/// \brief our factory, will setup and hold our resources
			OpenGLFactory factory;
			World* world;

			/// ]brief UBO for light information
			cgc::strong_ptr<UBO> lightsUBO;
			/// \brief UBO for camera information
			cgc::strong_ptr<UBO> cameraUBO;

			uint enabledTextures[32];

			uint query[2];
			bool queryID;

			// Debug/Dev Rendering
			cgc::strong_ptr<OpenGLShader> devRenderObjectsShader;

			RenderTechnique<OpenGLRenderData, OpenGLRenderer>* renderTechnique;
			RenderTechnique<OpenGLRenderData, OpenGLRenderer>* overlayRenderTechnique;

		public:
			uint32_t elapsedTime;
			

			/// \brief Constructor
			OpenGLRenderer(glm::uvec2 screenSize);
			/// \brief Destructor
			~OpenGLRenderer();

			/// \brief post constructor initialization
			virtual void Initialize() override { }
			virtual void ThreadInitialize() override;

			virtual void ReInitialize();

			/// \brief main frame render call
			/// \param renderList	list of objects that will be rendered (without culling)
			virtual void RenderFrame();
			/// \brief Renders all objects in the list, for development purposes
			/// \param renderObjects list of objects to render
			virtual void RenderDevObjects(const std::vector<DevRenderObject>& renderObjects);

			virtual uint GetRenderTime();

			virtual void SetThreadID(const std::thread::id& id);

			void ActivateTexture(uint textureIndex, std::size_t uniformVariable, Texture2D* texture, cgc::raw_ptr<OpenGLShader> shader);
			void ActivateTexture(uint textureIndex, std::size_t uniformVariable, Texture3D* texture, cgc::raw_ptr<OpenGLShader> shader);
			void ActivateTexture(uint textureIndex, std::size_t uniformVariable, TextureCube* texture, cgc::raw_ptr<OpenGLShader> shader);
			void ClearActiveTextures();

			inline const cgc::strong_ptr<UBO>& GetLightsUBO() { return lightsUBO; }
			inline const cgc::strong_ptr<UBO>& GetCameraUBO() { return cameraUBO; }

			inline OpenGLFactory& GetOpenGLFactory() { return factory; }
			inline const OpenGLFactory& GetOpenGLFactory() const { return factory; }

			virtual RenderingFactory& GetFactory() { return factory; }
			virtual const RenderingFactory& GetFactory() const { return factory; }

			virtual void SetWorld(World* world);
		};
	}
}