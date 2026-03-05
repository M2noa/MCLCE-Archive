#include "Lighting.h"
#include "../../OpenGLDebug.h"
#include "Math/Math.h"
#include <SFML/System.hpp>

namespace Esteem
{
	namespace OpenGL
	{
		Lighting::Lighting(OpenGLRenderer& renderer, const cgc::strong_ptr<OpenGLShader>& lightingShader, const std::unordered_map<std::string, cgc::strong_ptr<Texture2D>>& textures)
			: RenderTechnique(renderer)
			, lightingShader(lightingShader)
			, textures(textures)
			, indicesCount()
			, sphereVAO()
		{

		}

		void Lighting::Initialize()
		{			
			CreateSphereVAO(1.0f, 16, 16);
		}

		void Lighting::CreateSphereVAO(float radius, uint stacks, uint slices)
		{
			std::vector<glm::vec3> vertices;

			for (double stack = 0; stack < stacks; ++stack)
			{
				double lat0 = M_PI * (-0.5 + stack / stacks);
				double y0 = sin(lat0), yr0 = cos(lat0);

				double lat1 = M_PI * (-0.5 + (stack + 1) / stacks);
				double y1 = sin(lat1), yr1 = cos(lat1);

				for (double slice = 1; slice <= slices; ++slice)
				{
					double lng = 2 * M_PI * slice / slices;
					double x = cos(lng), z = sin(lng);

					vertices.push_back(glm::vec3(x * yr0, y0, z * yr0) * radius);
					vertices.push_back(glm::vec3(x * yr1, y1, z * yr1) * radius);
				}
			}

			glGenVertexArrays(1, &sphereVAO);
			glBindVertexArray(sphereVAO);

			uint32_t vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

			uint32_t vertexIndex = lightingShader->GetAttributeVariable("vertex")->index;
			glEnableVertexAttribArray(vertexIndex);
			glVertexAttribPointer(vertexIndex, 3, GL_FLOAT, false, sizeof(glm::vec3), 0);

			glBindVertexArray(0);

			indicesCount = vertices.size();
		}

		Lighting::~Lighting()
		{

		}


		void Lighting::RenderFrame(const OpenGLRenderData& renderData)
		{
			glDepthMask(GL_FALSE);
			glCullFace(GL_FRONT);

			//renderer->GetWorldData();

			lightingShader->Bind();
			if (textures.size() > 0)
			{
				uint32_t i = 0;
				for (auto& [name, texture] : textures)
					renderer.ActivateTexture(i++, RT_HASH(name), texture.ptr(), lightingShader.ptr());
			}

			for (auto& light : *renderData.GetLights())
			{
				lightingShader->SetUniform(CT_HASH("ModelMatrix"), *light.matrix);
				lightingShader->SetUniform(CT_HASH("lightID"), (int)light.index);

				switch (light.data->type)
				{
				case LightData::LightType::POINT:
					glBindVertexArray(sphereVAO);
					glDrawArrays(GL_TRIANGLE_STRIP, 0, indicesCount);
					glBindVertexArray(0);
					break;

				case LightData::LightType::SPOT:
					glBindVertexArray(sphereVAO);
					glDrawArrays(GL_TRIANGLE_STRIP, 0, indicesCount);
					glBindVertexArray(0);
					break;
				}
			}

			glCullFace(GL_BACK);
		}
	}
}