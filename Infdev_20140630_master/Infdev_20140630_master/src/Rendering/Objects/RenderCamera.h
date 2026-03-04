#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "General/Color.h"
#include <cppu/cgc/pointers.h>
#include <RmlUi/Core/Context.h>

namespace Esteem
{
	class GraphicalOverlay;

	struct RenderCameraData
	{
		glm::vec3 position;
		float nearClipDistance;
		glm::vec3 forward;
		float farClipDistance;
		glm::vec3 fogColor;
		float fogDistance;

		glm::mat4 viewProjectMatrix;
		glm::mat4 invertedViewProjectMatrix;
		glm::mat4 projectionMatrix;
		glm::mat4 orthoProjectionMatrix;
	};

	class RenderCamera
	{
	public:
		RenderCameraData data;
		glm::mat4 viewMatrix;

		std::vector<GraphicalOverlay*> graphicalOverlays; // GUI, HUD, anything that will be drawn on top

		Rml::Context* guiContext;
		std::vector<Rml::ElementDocument*> guiElements; // GUI, HUD, anything that will be drawn on top
		
		RenderCamera()
			: guiContext(nullptr)
		{
		}
		~RenderCamera() {}

		void AddGraphicalOverlay(cgc::raw_ptr<GraphicalOverlay> overlay);
		void RemoveGraphicalOverlay(cgc::raw_ptr<GraphicalOverlay> overlay);

		Rml::ElementDocument* LoadHTMLDocument(const std::string& path);
		void RemoveHTMLDocument(Rml::ElementDocument* document);
	};
}