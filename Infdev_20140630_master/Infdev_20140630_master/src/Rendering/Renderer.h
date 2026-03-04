#pragma once

#include <vector>
#include <thread>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

#include "./Objects/RenderObject.h"
#include "./RenderingFactory.h"
#include "./Objects/IRenderData.h"
#include "./Objects/DevRenderObject.h"
#include "Rendering/Objects/RenderCamera.h"

namespace Esteem
{
	class World;

	class Renderer
	{
	protected:
		glm::uvec2 screenSize;

		Renderer(glm::uvec2 screenSize)
			: screenSize(screenSize)
		{ }

	public:
		/// \brief Destructor
		virtual ~Renderer() = 0;
		/// \brief Set the screen size of this renderer
		inline void SetScreenSize(const glm::uvec2& screenSize) { this->screenSize = screenSize; }
		/// \brief Get the screen size of this renderer
		inline const glm::uvec2& GetScreenSize() const { return screenSize; }

		/// \brief
		virtual void Initialize() = 0;
		/// \brief
		virtual void ThreadInitialize() = 0;
		/// \brief
		virtual void ReInitialize() = 0;
		/// \brief
		virtual void RenderFrame() = 0;
		/// \brief
		virtual void RenderDevObjects(const std::vector<DevRenderObject>& renderObjects) = 0;

		virtual uint GetRenderTime() = 0;

		virtual void SetThreadID(const std::thread::id& id) = 0;

		virtual void SetWorld(World* world) = 0;

		virtual RenderingFactory& GetFactory() = 0;
		virtual const RenderingFactory& GetFactory() const = 0;
	};

	inline Renderer::~Renderer() {}
}