#pragma once

#include <vector>
#include <sstream>

#include <SFML/Window.hpp>

#include "Rendering/Renderer.h"
#include "Rendering/Objects/IRenderData.h"
#include "Window/IView.h"
#include "Input/Input.h"
#include "Audio/IAudioSystem.h"
#include "Window/Window.h"

#include <cppu/cgc/pointers.h>
#include "Utils/details/sfml_stream.h"

#include "Rendering/Objects/Text.h"

#include "Rendering/HTML/HTMLManager.h"
#include "Rendering/HTML/HTMLRendererOpenGL.h"
#include "Rendering/HTML/HTMLFileInterface.h"

#undef CreateWindow

namespace Esteem
{

	typedef delegate<void(sf::RenderWindow*)> PostRenderdelegate;

	class Camera;

	/// \brief Regular View, holds a renderer and handles input
	/// 
	/// The RenderView is the regular View of this game engine, it creates a window,
	/// creates a context to render with, handles input and other human interface devices.
	/// \note Use and design these Views as if they can be switched per application.
	/// \author Y. Schaeffer
	/// \date March 2015
	class RenderView : public IView
	{
		friend class Mouse;
	private:
		bool windowCreated;
		bool renderFramesPaused;

		bool isTogglingNewScreen;

		/// \brief True; if the window is focused
		///
		/// Without this check we will always be reading the input of the computer,
		/// ie: the user has alt-tabbed to his/her webbrowser and is typing or using the mouse.
		bool screenFocused;

		glm::ivec2 screenSize;
		glm::ivec2 screenCenterPosition;
		std::vector<ScreenSizedelegate> screenSizeListeners;

		sf::WindowHandle systemHandle;

#ifdef _WIN32
		RECT windowRect;
		HDC deviceContext;
#elif __APPLE__

#endif

		/// \brief Renderer
		std::unique_ptr<Renderer> renderer;

		/// \brief Main window
		std::unique_ptr<Window> window;
				
		std::chrono::high_resolution_clock::time_point fpsCountNextTime;	///< stores next time when we will check the frames again
		int frames;					///< frame base_container will increment every frame
		int fpsCount;				///< fps count of last second
		int elapsedMilliseconds;	///< timer to write milliseconds taken to render the previous frame
		float avgDeltaTime;			///< average delta time per second
		uint targetFps;

		std::vector<PostRenderdelegate> postRenders;

		bool mouseCursorVisible;

				
		/// \brief the world that we will render
		World* world;
		
		HTMLManager htmlManager;
		HTMLRendererOpenGL htmlRenderer;
		HTMLFileInterface htmlFileInterface;
		Rml::Context* htmlContext;

		/// \brief TODO: bring this to the input class
		void HandleEvents();

		/// \brief Initialize the renderer
		void InitializeRenderer();
		
		/// \brief Will be called every frame, this is the main rendering function
		void RenderFrame();

		/// \brief Creates the window
		void CreateWindow(sf::WindowHandle* windowHandle = nullptr);

	#pragma region Debug Rendering TODO: move this to another class
		cgc::strong_ptr<GraphicalOverlay> statisticsOverlay;
		cgc::strong_ptr<Text> fps;
		cgc::strong_ptr<Text> debug;

		std::stringstream devBoardText;

		void DrawStatisticsWindow();
		void UpdateStatisticsWindow();
	#pragma endregion

		virtual void LockCursorInScreen(bool lock);

	public:
		/// \brief Constructor, will initialize the window
		RenderView();
		/// \brief Constructor, will initialize the window
		RenderView(void* windowHandle);

		/// \brief Destructor
		~RenderView();

		/// \brief Closes the window
		/// \param abort        if true it will also issue an abort of the StartView() function
		void CloseWindow(bool abort = true);

		/// \brief To know if we have a focus on the screen instead of the borders use this to check
		/// \return True if the screen is focused
		virtual bool IsScreenFocused() { return screenFocused; }
		/// \brief is this window focused?
		/// \return True if this screen is focused
		inline bool IsFocused() const { return window->hasFocus(); }

		virtual void SetMouseCursorVisible(bool visible) { window->setMouseCursorVisible(mouseCursorVisible = visible); }
		virtual bool IsMouseCursorVisible() { return mouseCursorVisible; }

		void UpdateScreenRectangle();

		virtual const glm::ivec2& GetScreenSize() const { return screenSize; }
		virtual const glm::ivec2& GetScreenCenterPosition() const { return screenCenterPosition; }

		virtual void RegisterScreenSizeListener(const ScreenSizedelegate& function);
		virtual void UnRegisterScreenSizeListener(const ScreenSizedelegate& function);


		virtual const glm::ivec2 GetMousePosition() { sf::Vector2i mPos = sf::Mouse::getPosition(*window); return reinterpret_cast<glm::ivec2&>(mPos); };
		virtual void SetMousePosition(const glm::ivec2& position) { sf::Mouse::setPosition(reinterpret_cast<const sf::Vector2i&>(position), *window); };

		/// \brief Get pointer to the sfml render window
		/// return Pointer to render window
		sf::RenderWindow* GetWindow() const { return window.get(); }

		/// \brief Set and enable the fps limiter
		void SetFPSLimit(uint limit);

		/// \brief set the world that we will render
		virtual void SetWorld(World* world);
		
		/// \brief Function that will be called by the thread
		virtual void WindowUpdate();
		/// \brief Updates all rendering systems
		virtual void Update();
		/// \brief Render the frame
		virtual void Render();

		virtual RenderingFactory* GetFactory() { return renderer ? &renderer->GetFactory() : nullptr; };

		virtual void EnableStatisticsWindow(bool enable);

		/// \brief Create and save a screenshot of the screen
		void ScreenShot();

		void OnCommand(const std::string& command, const std::string& value);
		
		void RegisterPostRender(PostRenderdelegate delegate);
		void UnRegisterPostRender(PostRenderdelegate postRender);
	};
}