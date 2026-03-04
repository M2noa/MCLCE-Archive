#pragma once

#include <vector>
#include <unordered_map>
#include "../../OpenGLRenderer.h"
#include "../../OpenGLRenderData.h"
#include "../../Objects/OpenGLRenderObject.h"
#include "../../../../IRenderTechnique.h"
#include "../Shadowing/ShadowMap.h"

namespace Esteem
{
	namespace OpenGL
	{
		class Lighting : public RenderTechnique <OpenGLRenderData, OpenGLRenderer>
		{
		private:
			uint sphereVAO;
			cgc::strong_ptr<OpenGLShader> lightingShader;
			uint indicesCount;

			std::unordered_map<std::string, cgc::strong_ptr<Texture2D>> textures;

		public:
			Lighting(OpenGLRenderer& renderer, const cgc::strong_ptr<OpenGLShader>& lightingShader, const std::unordered_map<std::string, cgc::strong_ptr<Texture2D>>& textures);
			virtual ~Lighting();

			virtual void RenderFrame(const OpenGLRenderData& renderData);

			virtual void Initialize();
			void CreateSphereVAO(float radius, uint stacks, uint slices);

		};
	}
}