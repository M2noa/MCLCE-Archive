#include "CascadedShadowMap.h"

#include <glm/glm.hpp>
#include <sstream>

#include "Math/Math.h"
#include "Utils/Debug.h"
#include "General/Settings.h"

#include "World/Constituents/Camera.h"
#include "../../OpenGLFactory.h"

namespace Esteem
{
	namespace OpenGL
	{
		CascadedShadowMap::CascadedShadowMap(OpenGLRenderer& renderer, ShadowMap& shadowMapTechnique)
			: RenderTechnique(renderer)
			, shadowMapTechnique(shadowMapTechnique)
		{

		}

		void CascadedShadowMap::Initialize()
		{
			ConstructSettings::Texture2D settings;
			settings.size = Settings::cascadedShadowMapSize;
			settings.mipmapped = false;
			settings.repeat = TEXTURE_REPEAT::CLAMP_TO_BORDER;
			settings.textureFilter = TEXTURE_FILTER::LINEAR;
			settings.mipMapFilter = TEXTURE_FILTER::LINEAR_MIPMAP_LINEAR;
			settings.format = TEXTURE_FORMAT::RGBA_16F;
			settings.borderColor = Color(0, 0, 0, 0);

			ConstructSettings::Texture2D settingsDepth;
			settingsDepth.size = Settings::cascadedShadowMapSize;
			settingsDepth.mipmapped = false;
			settingsDepth.repeat = TEXTURE_REPEAT::CLAMP_TO_BORDER;
			settingsDepth.textureFilter = TEXTURE_FILTER::LINEAR;
			settingsDepth.mipMapFilter = TEXTURE_FILTER::LINEAR_MIPMAP_LINEAR;
			settingsDepth.format = TEXTURE_FORMAT::DEPTH_16U;
			settingsDepth.borderColor = Color(0, 0, 0, 0);

			cascadedShadowTexture = renderer.GetOpenGLFactory().LoadTexture2D(settings, "@CascadedShadowTexture");
			cgc::strong_ptr<Texture2D> depthTexture = renderer.GetOpenGLFactory().LoadTexture2D(settingsDepth, "@CascadedDepthTexture");

			cascadedShadowFBO = renderer.GetOpenGLFactory().LoadFBO("CascadedShadowFBO");
			cascadedShadowFBO->AttachDepthTexture(depthTexture);
			cascadedShadowFBO->AttachRenderTexture(cascadedShadowTexture, 0);
			cascadedShadowFBO->Validate();


			float aspect = 1920.f / 1080.f;
			CalculateCascadedDistances(4, 4.f, glm::radians(45.f), 1.f, 100.f, 4.f);

			renderNextIndex = 0;
		}

		void CascadedShadowMap::CalculateCascadedDistances(uint numberOfCascades, float firstRadius, float fov, float nearDistance, float farDistance, float increment)
		{
			glm::vec2 linePoint1 = glm::vec2(0, 0);
			//glm::vec2 linePoint2 = glm::vec2(cos(fov), sin(fov));
			glm::vec2 linePoint2 = glm::vec2(cos(fov), sin(fov));

			float r = firstRadius;
			float rStart = r;

			if (nearDistance < std::numeric_limits<float>::min())
				nearDistance = std::numeric_limits<float>::min();

			// start with near plane offset
			glm::vec2 intersectionPoint;
			intersectionPoint.y = nearDistance;
			intersectionPoint.x = tan(fov * 0.5) * intersectionPoint.y;

			// calculate all circle positions
			std::vector<float> circleDistances;
			circleDistances.resize(numberOfCascades);
			for(uint i = 0; i < numberOfCascades; ++i)
			{
				circleDistances[i] = intersectionPoint.y + sqrt((r * r) - (intersectionPoint.x * intersectionPoint.x));
				intersectionPoint = Math::CalculateCircleLineIntersectionPoints(glm::vec2(0, circleDistances[i]), r, linePoint1, linePoint2)[1];
				r *= increment;
			}

			// scale all positions and radii to fill the whole farDistance
			uint sphereSize = 2;
			cascadedDistances.clear();
			float distance = intersectionPoint.y;
			for(uint i = 0; i < numberOfCascades; ++i)
			{
				cascadedDistances.emplace_back(
					(rStart / distance) * farDistance,
					100.f * pow(i + 1, 2),
					(circleDistances[i] / distance) * farDistance
				);
				rStart *= increment;
			}

			cascadedMatrices.resize(numberOfCascades);
		}

		void CascadedShadowMap::RenderFrame(const OpenGLRenderData& renderData)
		{

		}

		void CascadedShadowMap::RenderFrame(const OpenGLRenderData& renderData, const glm::mat4& lightMatrix, const glm::vec3& lightDirection)
		{
			glDepthMask(GL_TRUE);
			glDepthFunc(GL_GEQUAL);
			glClearDepth(0.f);

			cascadedShadowFBO->Bind();
			glClearColor(0.f, 0.f, 0.f, 0.f);

			/*int i = renderNextIndex++;
			if(renderNextIndex >= cascadedDistances.size())
				renderNextIndex = 0;*/

			//glm::mat4 nLightMatrix = glm::transpose(lightMatrix);
			glm::mat4 invLightMatrix = glm::inverse(lightMatrix);

			for(uint i = 0; i < cascadedDistances.size(); ++i)
			{
				float radius = cascadedDistances[i].x;
				float height = cascadedDistances[i].y;
				float offset = cascadedDistances[i].z;

				float pixelsPerUnit = (radius * 2.f) / (float)Settings::cascadedShadowMapSize.x;
				float unitsPerPixel = 1.f / pixelsPerUnit;

				RenderCameraData& renderCamera = renderData.GetCamera()->GetRenderCameraData()->data;
				glm::vec4 center(
					renderCamera.position
					+ (renderCamera.forward * offset)
					, 1.f);

				/*glm::mat4 lMatrix = glm::lookAt(glm::vec3(center), glm::vec3(center) + lightDirection, glm::vec3(0, 1, 0));
				glm::mat4 invLMatrix = glm::inverse(lMatrix);

				center = lMatrix * center;
				center.x = round(center.x * unitsPerPixel) * pixelsPerUnit;
				center.y = round(center.y * unitsPerPixel) * pixelsPerUnit;
				center.z -= radius;
				center = invLMatrix * center;*/


				center = lightMatrix * center;
				center.x = round(center.x * unitsPerPixel) * pixelsPerUnit;
				center.y = round(center.y * unitsPerPixel) * pixelsPerUnit;
				center.z -= (height * 2.f) - radius;
				center = invLightMatrix * center;


				glm::mat4 m = glm::ortho(-radius, radius, -radius, radius, height, 0.f);
				m = glm::translate(m * lightMatrix, glm::vec3(-center));

				cascadedMatrices[i] = m;

				glColorMask(i == 0, i == 1, i == 2, i == 3);
				glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

				shadowMapTechnique.RenderFrame(renderData, cascadedMatrices[i], 1.f, true);
			}

			glColorMask(true, true, true, true);
			glDepthFunc(GL_LEQUAL);
			glClearDepth(1.f);
			FBO::UnBind(renderer);
		}
	}
}