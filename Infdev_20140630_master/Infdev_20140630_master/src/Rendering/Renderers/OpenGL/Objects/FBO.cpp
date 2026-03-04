#include "FBO.h"
#include <GL/glew.h>
#include "Utils/Debug.h"
#include "../OpenGLRenderer.h"
#include "General/Settings.h"

namespace Esteem
{
	namespace OpenGL
	{
		uint FBO::bindID = 0; // 0 is default (non bind)
		FBO* FBO::current = nullptr;

		FBO::FBO(uint id, const std::string& name)
			: name(name)
			, validated(false)
			, id(id) // 0 is default (non bind)
			, attachedDepthTexture()
			, attachedStencilTexture()
			, attachedDepth3DTexture()
			, attachedStencil3DTexture()
			, attachedDepthBuffer()
			, attachedStencilBuffer()
		{ }

		FBO::~FBO()
		{
			if (FBO::bindID == id)
				FBO::InternalUnBind();

			glDeleteFramebuffers(1, &id);
		}

		void FBO::AttachRenderTexture(const cgc::strong_ptr<Texture2D>& colorTexture, const uint8 index)
		{
			if (index > 15)
				Debug::LogError("FBO::AttachRenderTexture(Texture2D* colorTexture, uint8 index), index can not be higher than 15");

			InternalBind();
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, colorTexture->GetID(), 0);

			if (std::find(buffers.begin(), buffers.end(), GL_COLOR_ATTACHMENT0 + index) == buffers.end())
			{
				buffers.push_back(GL_COLOR_ATTACHMENT0 + index);
				glDrawBuffers(buffers.size(), &buffers[0]);
			}

			renderSize = colorTexture->GetSettings().size;
			attachedRenderTextures[index] = colorTexture;

			FBO::InternalUnBind();
			validated = false;
		}

		void FBO::AttachDepthTexture(const cgc::strong_ptr<Texture2D>& depthTexture)
		{
			InternalBind();
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture->GetID(), 0);

			renderSize = depthTexture->GetSettings().size;
			attachedDepthTexture = depthTexture;

			FBO::InternalUnBind();
			validated = false;
		}

		void FBO::AttachStencilTexture(const cgc::strong_ptr<Texture2D>& stencilTexture)
		{
			InternalBind();
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, stencilTexture->GetID(), 0);

			renderSize = stencilTexture->GetSettings().size;
			attachedStencilTexture = stencilTexture;

			FBO::InternalUnBind();
			validated = false;
		}

		void FBO::AttachRenderTexture(const cgc::strong_ptr<Texture3D>& colorTexture, const uint8 index)
		{
			if (index > 15)
				Debug::LogError("FBO::AttachRenderTexture(Texture3D* colorTexture, uint8 index), index can not be higher than 15");

			InternalBind();
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_3D, colorTexture->GetID(), 0);

			if (std::find(buffers.begin(), buffers.end(), GL_COLOR_ATTACHMENT0 + index) == buffers.end())
				buffers.push_back(GL_COLOR_ATTACHMENT0 + index);

			renderSize = colorTexture->GetSize();
			attachedRender3DTextures[index] = colorTexture;

			glDrawBuffers(buffers.size(), &buffers[0]);

			FBO::InternalUnBind();
			validated = false;
		}

		void FBO::AttachDepthTexture(const cgc::strong_ptr<Texture3D>& depthTexture)
		{
			InternalBind();
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_3D, depthTexture->GetID(), 0);

			renderSize = depthTexture->GetSize();
			attachedDepth3DTexture = depthTexture;

			FBO::InternalUnBind();
			validated = false;
		}

		void FBO::AttachStencilTexture(const cgc::strong_ptr<Texture3D>& stencilTexture)
		{
			InternalBind();
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_3D, stencilTexture->GetID(), 0);

			renderSize = stencilTexture->GetSize();
			attachedStencil3DTexture = stencilTexture;

			FBO::InternalUnBind();
			validated = false;
		}

		void FBO::AttachRenderBuffer(const cgc::strong_ptr<RBO>& colorBuffer, const uint8 index)
		{
			if (index > 15)
				Debug::LogError("FBO::AttachRenderBuffer(RBO* colorBuffer, uint8 index), index can not be higher than 15");

			InternalBind();
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_RENDERBUFFER, colorBuffer->GetID());

			if (std::find(buffers.begin(), buffers.end(), GL_COLOR_ATTACHMENT0 + index) == buffers.end())
				buffers.push_back(GL_COLOR_ATTACHMENT0 + index);

			renderSize = colorBuffer->GetSize();
			attachedRenderBuffers[index] = colorBuffer;

			glDrawBuffers(buffers.size(), &buffers[0]);

			FBO::InternalUnBind();
			validated = false;
		}

		void FBO::AttachDepthBuffer(const cgc::strong_ptr<RBO>& depthBuffer)
		{
			InternalBind();
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer->GetID());

			renderSize = depthBuffer->GetSize();
			attachedDepthBuffer = depthBuffer;

			FBO::InternalUnBind();
			validated = false;
		}

		void FBO::AttachStencilBuffer(const cgc::strong_ptr<RBO>& stencilBuffer)
		{
			InternalBind();
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, stencilBuffer->GetID());

			renderSize = stencilBuffer->GetSize();
			attachedStencilBuffer = stencilBuffer;

			FBO::InternalUnBind();
			validated = false;
		}

		bool FBO::Validate()
		{
			InternalBind();

			GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if (status == GL_FRAMEBUFFER_COMPLETE)
				validated = true;
			else
				Debug::LogError("FBO: \"" + name + "\" is not valid, status=" + std::to_string(status));

			FBO::InternalUnBind();

			return status == GL_FRAMEBUFFER_COMPLETE;
		}

		void FBO::InternalBind()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, id);
			FBO::bindID = id;
		}

		void FBO::Bind() const
		{
			if (validated)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, id);
				glViewport(0, 0, renderSize.x, renderSize.y);

				FBO::current = const_cast<FBO*>(this);
				FBO::bindID = id;
			}
		}

		void FBO::InternalUnBind()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			FBO::bindID = 0;
			FBO::current = nullptr;
		}

		void FBO::UnBind(const OpenGLRenderer& renderer)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, renderer.GetScreenSize().x, renderer.GetScreenSize().y);

			FBO::bindID = 0;
			FBO::current = nullptr;
		}
	}
}
