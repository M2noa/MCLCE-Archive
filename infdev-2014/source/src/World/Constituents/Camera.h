#pragma once

#include "stdafx.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <SFML/Window/Mouse.hpp>

#include "Rendering/Objects/RenderCamera.h"
#include "Rendering/Objects/GraphicalOverlay.h"
#include "./AbstractConstituent.h"
#include <cppu/cgc/array.h>

namespace Esteem
{
	class Camera : public AbstractConstituent
	{
		// Only RenderingFactory can create this constituent
		friend class RenderingFactory;
	private:
		cgc::strong_ptr<RenderCamera> renderCamera;
		bool enabled;

	public:

		Camera(const cgc::strong_ptr<Entity>& entity, const cgc::strong_ptr<RenderCamera>& renderCameraData)
			: AbstractConstituent(entity)
			, renderCamera(renderCameraData)
		{
			Initialize();
		}
		
		~Camera();

		static cgc::strong_ptr<Camera> Instantiate(const cgc::strong_ptr<Entity>& entity);

		void Initialize();
		void Update();
		void DirtyCleanUp();

		void OnScreenSizeChanged(const glm::ivec2& screenSize);

		bool GetEnabled() const { return enabled; }
		void SetEnabled(bool enable) { enabled = enable; }

		inline const cgc::strong_ptr<RenderCamera>& GetRenderCameraData() { return renderCamera; }
		void AddGraphicOverlay(const cgc::strong_ptr<GraphicalOverlay>& graphicOverlay);
		void RemoveGraphicOverlay(const cgc::strong_ptr<GraphicalOverlay>& graphicOverlay);

		Rml::ElementDocument* LoadHTMLDocument(const std::string& path);
		void RemoveHTMLDocument(Rml::ElementDocument* document);
	};
}
