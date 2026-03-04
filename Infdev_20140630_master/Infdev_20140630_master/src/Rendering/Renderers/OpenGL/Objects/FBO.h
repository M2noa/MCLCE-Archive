#pragma once

#include "stdafx.h"
#include <string>
#include <vector>
#include <map>
#include <cppu/cgc/pointers.h>

#include "Rendering/Objects/Texture3D.h"
#include "Rendering/Objects/Texture2D.h"
#include "./RBO.h"

namespace Esteem
{
	namespace OpenGL
	{
		class OpenGLRenderer;

		class FBO
		{
		friend class OpenGLFactory;
		friend class cgc::constructor;

		private:
			std::string name;
			bool validated;
			uint id;
			static FBO* current;

			std::vector<GLenum> buffers;
			glm::uvec2 renderSize;

			std::map<uint, cgc::strong_ptr<Texture2D>> attachedRenderTextures;
			cgc::strong_ptr<Texture2D> attachedDepthTexture;
			cgc::strong_ptr<Texture2D> attachedStencilTexture;

			std::map<uint, cgc::strong_ptr<Texture3D>> attachedRender3DTextures;
			cgc::strong_ptr<Texture3D> attachedDepth3DTexture;
			cgc::strong_ptr<Texture3D> attachedStencil3DTexture;

			std::map<uint, cgc::strong_ptr<RBO>> attachedRenderBuffers;
			cgc::strong_ptr<RBO> attachedDepthBuffer;
			cgc::strong_ptr<RBO> attachedStencilBuffer;

			FBO(uint id, const std::string& name);
			void InternalBind();
			void InternalUnBind();

		public:
			~FBO();

			void AttachRenderTexture(const cgc::strong_ptr<Texture2D>& colorTexture, const uint8 index);
			void AttachDepthTexture(const cgc::strong_ptr<Texture2D>& depthTexture);
			void AttachStencilTexture(const cgc::strong_ptr<Texture2D>& stencilTexture);
			
			void AttachRenderTexture(const cgc::strong_ptr<Texture3D>& colorTexture, const uint8 index);
			void AttachDepthTexture(const cgc::strong_ptr<Texture3D>& depthTexture);
			void AttachStencilTexture(const cgc::strong_ptr<Texture3D>& stencilTexture);

			void AttachRenderBuffer(const cgc::strong_ptr<RBO>& colorBuffer, const uint8 index);
			void AttachDepthBuffer(const cgc::strong_ptr<RBO>& depthBuffer);
			void AttachStencilBuffer(const cgc::strong_ptr<RBO>& stencilBuffer);

			/// \brief build this framebuffer
			bool Validate();

			inline std::string GetName() const { return name; }
			inline uint GetID() const { return id; }

			void Bind() const;

			inline static FBO* GetCurrent() { return current; }
			static void UnBind(const OpenGLRenderer& renderer);
			static uint bindID;
		};
	}
}