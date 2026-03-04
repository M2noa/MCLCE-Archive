#include "SkyBox.h"
#include "../OpenGLDebug.h"
#include "Math/Math.h"

#include "../OpenGLRenderer.h"
#include "../OpenGLRenderData.h"
#include "../OpenGLFactory.h"
#include "../Objects/OpenGLRenderObject.h"

#include "Rendering/Objects/Image.h"

#include "Utils/Diagnostics.h"

namespace Esteem
{
	namespace OpenGL
	{
		SkyBox::SkyBox(OpenGLRenderer& renderer)
			: RenderTechnique(renderer)
		{

		}

		void SkyBox::Initialize()
		{
			ConstructSettings::TextureCube skyboxTextureSettings;

			std::vector<cgc::strong_ptr<Image>> skyboxTextures;
			/*skyboxTextures.push_back("SKYBOX/hw_hourglass/hourglass_rt");
			skyboxTextures.push_back("SKYBOX/hw_hourglass/hourglass_lf");
			skyboxTextures.push_back("SKYBOX/hw_hourglass/hourglass_up");
			skyboxTextures.push_back("SKYBOX/hw_hourglass/hourglass_dn");
			skyboxTextures.push_back("SKYBOX/hw_hourglass/hourglass_bk");
			skyboxTextures.push_back("SKYBOX/hw_hourglass/hourglass_ft");*/
			skyboxTextures.push_back(cgc::construct_new<Image>("SKYBOX/water_clouds/sea_clouds_right", false));
			skyboxTextures.push_back(cgc::construct_new<Image>("SKYBOX/water_clouds/sea_clouds_left", false));
			skyboxTextures.push_back(cgc::construct_new<Image>("SKYBOX/water_clouds/sea_clouds_top", false));
			skyboxTextures.push_back(cgc::construct_new<Image>("SKYBOX/water_clouds/sea_clouds_bottom", false));
			skyboxTextures.push_back(cgc::construct_new<Image>("SKYBOX/water_clouds/sea_clouds_front", false));
			skyboxTextures.push_back(cgc::construct_new<Image>("SKYBOX/water_clouds/sea_clouds_back", false));
			/*skyboxTextures.push_back("SKYBOX/night_clouds/night_clouds_right");
			skyboxTextures.push_back("SKYBOX/night_clouds/night_clouds_left");
			skyboxTextures.push_back("SKYBOX/night_clouds/night_clouds_top");
			skyboxTextures.push_back("SKYBOX/night_clouds/night_clouds_bottom");
			skyboxTextures.push_back("SKYBOX/night_clouds/night_clouds_front");
			skyboxTextures.push_back("SKYBOX/night_clouds/night_clouds_back");*/
			skyBoxTexture = renderer.GetOpenGLFactory().LoadTextureCube(skyboxTextureSettings, "@SkyBox", skyboxTextures);
			skyBoxShader = renderer.GetOpenGLFactory().LoadOpenGLShader("skyBox");

			// Vertex Data
			std::vector<ModelVertexDataA> data;

			std::vector<glm::vec3> vertices;
			vertices.push_back(glm::vec3(-1.0f, -1.0f, 1.0f));
			vertices.push_back(glm::vec3(1.0f, -1.0f, 1.0f));
			vertices.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
			vertices.push_back(glm::vec3(-1.0f, 1.0f, 1.0f));
			vertices.push_back(glm::vec3(-1.0f, -1.0f, -1.0f));
			vertices.push_back(glm::vec3(1.0f, -1.0f, -1.0f));
			vertices.push_back(glm::vec3(1.0f, 1.0f, -1.0f));
			vertices.push_back(glm::vec3(-1.0f, 1.0f, -1.0f));

			for(uint i = 0; i < vertices.size(); ++i)
			{
				ModelVertexDataA vertexData;
				vertexData.position = vertices[i];
				data.push_back(vertexData);
			}

			// Element Indices for the Cube
			std::vector<uint8> indices;

			indices.push_back(5); indices.push_back(4); indices.push_back(1); indices.push_back(1); indices.push_back(4); indices.push_back(0); // y-
			indices.push_back(2); indices.push_back(3); indices.push_back(6); indices.push_back(7); indices.push_back(6); indices.push_back(3); // y+
			indices.push_back(1); indices.push_back(0); indices.push_back(2); indices.push_back(3); indices.push_back(2); indices.push_back(0); // z-
			indices.push_back(6); indices.push_back(7); indices.push_back(5); indices.push_back(4); indices.push_back(5); indices.push_back(7); // z+
			indices.push_back(0); indices.push_back(4); indices.push_back(3); indices.push_back(7); indices.push_back(3); indices.push_back(4); // x-
			indices.push_back(5); indices.push_back(1); indices.push_back(6); indices.push_back(2); indices.push_back(6); indices.push_back(1); // x+
			
			cgc::strong_ptr<EBO> ebo = renderer.GetOpenGLFactory().LoadEBO(indices);
			cgc::strong_ptr<VBO> vbo = renderer.GetOpenGLFactory().LoadVBO(data);
			vbo->AddVertexAttribute("vertex", 3, GL_FLOAT, sizeof(ModelVertexDataA), (void*)offsetof(ModelVertexDataA, position));

			if(skyBoxShader->IsInitialized())
				vbo->SetShader(skyBoxShader);
			else
				Debug::LogError("SkyBox::Initialize(): can not attach to shader");

			vao = renderer.GetOpenGLFactory().LoadVAO();
			vao->AttachVBO(vbo);
			vao->AttachEBO(ebo);
		}

		SkyBox::~SkyBox()
		{
		}


		void SkyBox::RenderFrame(const OpenGLRenderData& renderData)
		{
			//glEnable(GL_DEPTH_TEST);
			//glDisable(GL_DEPTH_TEST);
			//glDepthMask(GL_FALSE);
			//glCullFace(GL_FRONT);

			skyBoxShader->Bind();
			vao->Bind();
			renderer.ActivateTexture(0, CT_HASH("baseTexture"), skyBoxTexture.ptr(), skyBoxShader.ptr());
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0); // RenderObject or not?

			Diagnostics::drawCalls++;

			//OpenGLShader::UnBind();

			//glCullFace(GL_BACK);
			//glDepthMask(GL_TRUE);
			//glEnable(GL_DEPTH_TEST);
		}
	}
}