#include "./Settings.h"

namespace Esteem
{
	std::string Settings::applicationName = "Esteem Proto (Game Engine)";
	glm::uvec2 Settings::initialScreenSize = glm::uvec2(1280, 720);
	bool Settings::vSyncEnabled = true;

	Settings::ScreenState Settings::screenState = Settings::ScreenState::WINDOWED;
	Settings::RendererType Settings::rendererType = Settings::RendererType::OPENGL;
	Settings::RenderMode Settings::renderMode = Settings::RenderMode::FORWARD_RENDERING;
	//Settings::RenderMode Settings::renderMode = Settings::RenderMode::LIGHT_INDEXED_RENDERING;
	//Settings::RenderMode Settings::renderMode = Settings::RenderMode::DEFERRED_RENDERING;

	TEXTURE_FILTER Settings::textureFlter = TEXTURE_FILTER::NEAREST;

	glm::uvec2 Settings::shadowAtlasSize = glm::uvec2(1024, 1024);
	glm::uvec2 Settings::cascadedShadowMapSize = glm::uvec2(1024, 1024);

	bool Settings::drawLines = false;
	bool Settings::drawDebug = false;
	bool Settings::drawCollisionBoxes = false;

	Settings::PhysicsEngine Settings::physicsEngine = Settings::PhysicsEngine::BULLET;
}