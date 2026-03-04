#pragma once

#include "stdafx.h"
#include <string>
#include <glm/glm.hpp>

#include "Rendering/rtypes.h"

namespace Esteem
{
	/// \brief Utility class that stores all of the settings of this program
	/// \author Y. Schaeffer
	/// \date March 2015
	class Settings
	{
	public:
		/// \brief name of this application
		static std::string applicationName;

		/// \brief preferred initial size of the screen (resolution)
		static glm::uvec2 initialScreenSize;

		/// \brief current size of the screen (resolution)
		static glm::uvec2 screenSize;

		static bool vSyncEnabled;

		/// \brief should the screen go fullscreen, windowed or windowed_fullscreen
		static enum class ScreenState
		{
			FULLSCREEN = 0,
			WINDOWED = 1,
			WINDOWED_FULLSCREEN = 2
		} screenState;

		/// \brief Renderer
		static enum class RendererType
		{
			OPENGL = 0,
			DIRECTX = 1,
			COUNT
		} rendererType;


		/// \brief Render modes
		static enum class RenderMode
		{
			FORWARD_RENDERING,
			DEFERRED_RENDERING,
			LIGHT_INDEXED_RENDERING
		} renderMode;

		static TEXTURE_FILTER textureFlter;


		/// \brief force drawing wireframe
		static bool drawLines;
		static bool drawDebug;
		static bool drawCollisionBoxes;

		/// \brief 
		static glm::uvec2 shadowAtlasSize;
		static glm::uvec2 cascadedShadowMapSize;

		enum PhysicsEngine
		{
			BULLET = 0
		};
		
		static PhysicsEngine physicsEngine;

		// Culling
		static constexpr size_t CullingOctreeMaxGridSize = 1024 * 2 * 2 * 2;
		static constexpr size_t CullingOctreeMinGridSize = 32;
		static constexpr size_t CullingOctreeDivideCount = 10;
		static constexpr size_t CullingOctreeMergeCount = 6;
	};
}