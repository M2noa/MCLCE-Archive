#include "RenderView.h"

#include <cppu/os/Statistics.h>
#include <cppu/os/ExceptionHandler.h>

#include "GameEngine.h"
#include "Utils/Data.h"
#include "Client/Console.h"

#include "Window/View.h"
#include "Rendering/Objects/Font.h"
#include "Rendering/Objects/Text.h"
#include "Rendering/Renderers/OpenGL/OpenGLRenderer.h"
#include "Rendering/Renderers/OpenGL/Objects/OpenGLRenderObject.h"
#include "General/Settings.h"

#include "Utils/Debug.h"
#include "Utils/Time.h"
#include "Utils/Diagnostics.h"
#include "General/Command.h"

#include "World/World.h"

#include <time.h>
#include <iomanip>
#include <filesystem>

#ifndef _WIN32
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif
#include <stb/stb_image_write.h>

#undef CreateWindow

namespace Esteem
{
	RenderView::RenderView()
		: RenderView(nullptr)
	{ }

	/// \brief Constructor, will initialize the window
	RenderView::RenderView(void* windowHandle)
		: renderer(nullptr)
		, frames(0)
		, fpsCount(0)
		, targetFps(60)
		, mouseCursorVisible(true)
		, isTogglingNewScreen(false)
		, htmlRenderer(nullptr)
	{
		View::SetView(this);
		screenSize = Settings::initialScreenSize;
		CreateWindow(static_cast<sf::WindowHandle*>(windowHandle));
		windowCreated = true;


		htmlRenderer.SetWindow(window.get());
		Rml::SetRenderInterface(&htmlRenderer);
		Rml::SetSystemInterface(&htmlManager);
		Rml::SetFileInterface(&htmlFileInterface);


		if (!Rml::Initialize())
			Debug::LogError("Could not initialize HTML renderer");

		Rml::LoadFontFace("./resources/fonts/consolas.ttf", true);
		Rml::LoadFontFace("./resources/fonts/superstar.ttf", false);

		// Initialize renderer
		InitializeRenderer();

		Command::RegisterListener("cl_vsync", DELEGATE(&RenderView::OnCommand, this));
		Command::RegisterListener("cl_fpscap", DELEGATE(&RenderView::OnCommand, this));
		Command::RegisterListener("cl_wireframe", DELEGATE(&RenderView::OnCommand, this));
		Command::RegisterListener("cl_draw_debug", DELEGATE(&RenderView::OnCommand, this));
		Command::RegisterListener("cl_draw_collision_boxes", DELEGATE(&RenderView::OnCommand, this));

		Command::RegisterListener("cl_refresh_hud", DELEGATE(&RenderView::OnCommand, this));
	}

	RenderView::~RenderView()
	{
		Rml::Shutdown();
		View::UnRegisterView(this);
	}

	void RenderView::CreateWindow(sf::WindowHandle* windowHandle)
	{
		// configure video mode
		sf::VideoMode videoMode;
		videoMode = Settings::screenState == Settings::ScreenState::WINDOWED_FULLSCREEN
			? sf::VideoMode().getDesktopMode()
			: videoMode = sf::VideoMode(screenSize.x, screenSize.y);

		// configure style
		sf::Uint32 screenStyle = sf::Style::None;
		if (Settings::screenState == Settings::ScreenState::WINDOWED)
			screenStyle |= sf::Style::Close | sf::Style::Resize;
		else if (Settings::screenState == Settings::ScreenState::FULLSCREEN)
			screenStyle |= sf::Style::Fullscreen;

		// create window
#ifdef __APPLE__
		const sf::ContextSettings::Attribute openGLMode = sf::ContextSettings::Attribute::Core;
#else
		const sf::ContextSettings::Attribute openGLMode = sf::ContextSettings::Attribute::Default;
#endif
		sf::ContextSettings contextSettings = sf::ContextSettings(24, 8, 0, 3, 3, openGLMode);
		if (windowHandle != nullptr)
		{
			sf::WindowHandle handle = (sf::WindowHandle)windowHandle;
			window = std::make_unique<Window>(handle, contextSettings);
		}
		else
			window = std::make_unique<Window>(videoMode, Settings::applicationName, screenStyle, contextSettings);

		//window->setFramerateLimit(60);

		// set our window as main for the mouse
		Input::GetMouse().SetView(this, window.get());

		window->setVerticalSyncEnabled(Settings::vSyncEnabled);
		screenFocused = window->hasFocus();

		if (Settings::vSyncEnabled)
			GameEngine::EnableFPSLimit(false);

		// get system handle
		systemHandle = window->getSystemHandle();
		UpdateScreenRectangle();

		screenSize = glm::ivec2(Settings::initialScreenSize.x, Settings::initialScreenSize.y);
		screenCenterPosition = glm::vec2(screenSize) * 0.5f;
	}

	void RenderView::CloseWindow(bool abort)
	{
		GameEngine::QuitEngine();
		//window->close();

		// we might just want to restart the view and keep the world and other data loaded.
		// TODO: find out how we can do this
		//if (abort)
		//	GameEngine::QuitEngine();
	}

	void RenderView::InitializeRenderer()
	{
		switch (Settings::rendererType)
		{
		case Settings::RendererType::DIRECTX:
			throw("RenderView::InitializeRenderer() DIRECTX as a rendering engine (Settings::RendererType) is not yet implemented");
			break;

		default: // OPENGL
			renderer = std::make_unique<OpenGL::OpenGLRenderer>(Settings::initialScreenSize);
			break;
		}


		// Post constructor initialization
		renderer->Initialize();

		// reactivate the opengl context in this thread
		window->setActive();
#ifdef _WIN32
		deviceContext = GetDC(window->getSystemHandle());
#endif

		renderer->SetThreadID(std::this_thread::get_id());
		renderer->ThreadInitialize();


		cgc::strong_ptr<Font> font = Font::LoadFont("./resources/fonts/consolas.ttf");

		statisticsOverlay = cgc::construct_new<GraphicalOverlay>("Statistics");
		statisticsOverlay->SetOrder(std::numeric_limits<int>::max() - 1);

		fps = cgc::construct_new<Text>("", font);
		fps->SetFontSize(11);
		fps->color = Color8u::white;
		fps->position = glm::vec3(15.f, 10.f, 0.f);

		debug = cgc::construct_new<Text>("", font);
		debug->SetFontSize(11);
		debug->position = glm::vec3(15.f, screenSize.y - 120.f, 0.f);


		/*rect = sf::RectangleShape(sf::Vector2f(200, 240));
		rect.setFillColor(sf::Color::Color(0, 0, 0, 220));
		rect.setOutlineThickness(1);
		rect.setOutlineColor(sf::Color::Color(180, 180, 180, 255));
		rect.setPosition(35, 5);*/

		devBoardText = std::stringstream();


		htmlContext = Rml::CreateContext("ScreenView", Rml::Vector2i(screenSize.x, screenSize.y));
		Console::Initialize(window.get(), htmlContext);
		//Console::SetRect(glm::vec4(0, 0, screenSize.x, 300), (glm::vec2)screenSize);
	}

	void RenderView::SetWorld(World* world)
	{
		this->world = world;
		renderer->SetWorld(world);
	}

#pragma region Execute, Pause and Stop this View
	void RenderView::WindowUpdate()
	{
		// Handle input
		HandleEvents();

		// window specific commands
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::F4))
				CloseWindow();

			bool toggleFullScreen = sf::Keyboard::isKeyPressed(sf::Keyboard::Return);
			if (toggleFullScreen && !isTogglingNewScreen)
				window->FullScreen(!window->IsFullScreen());
			isTogglingNewScreen = toggleFullScreen;
		}
	}

#pragma endregion

	void RenderView::Update()
	{
		WorldDataConstituents& constituents = world->GetWorldConstituents();

		constituents.lights.Update();
		//constituents.meshRenderers.Update();
		//constituents.cameras.Update();
	}

	void RenderView::Render()
	{
		if (window->isOpen())
			RenderFrame();
	}

	void RenderView::ScreenShot()
	{
		bool alpha = false;

		sf::Vector2u size = window->getSize();

		std::vector<uint8> pixels;
		uint widthMemSize = (3 + alpha) * size.x;
		pixels.resize(widthMemSize * size.y);
		uint8* pixelsStart = &pixels[0];

		glReadPixels(0, 0, size.x, size.y, alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, pixelsStart);

		// flip vertically
		uint8* tmp = new uint8[widthMemSize];
		uint sizeY = size.y - 1;
		uint sizeHalf = size.y / 2;
		for (uint h = 0; h < sizeHalf; ++h)
		{
			uint8* bgn = pixelsStart + h * widthMemSize;
			uint8* end = pixelsStart + (sizeY - h) * widthMemSize;

			memcpy(tmp, bgn, widthMemSize); // begin to temp
			memcpy(bgn, end, widthMemSize); // end to begin
			memcpy(end, tmp, widthMemSize); // temp to end
		}
		delete[] tmp;

		std::time_t t = std::time(0);   // get time now
		std::tm timeinfo;
#ifdef _WIN32
		localtime_s(&timeinfo, &t);
#else
		timeinfo = *std::localtime(&t); // returns pointer to static field, no need to clean up afterwards
#endif

		char buffer[20];
		strftime(buffer, 20, "%F %H.%M.%S", &timeinfo);

		std::string rootDir = "./";

#ifdef __APPLE__
		char* homePath = std::getenv("HOME");
		if (homePath != nullptr)
			rootDir = std::string(homePath) + "/Library/Application Support/Craftopia/";
		else
		{
			Debug::LogError("Could not find user's home directory");
			return;
		}
#endif

		std::string folder = rootDir + "screenshots/";
		std::string filename(folder + std::string(buffer) + ".png");

		std::error_code errorCode;
		if (std::filesystem::is_directory(folder, errorCode) || std::filesystem::create_directories(folder, errorCode) || errorCode.value() == 0)
		{

			if (stbi_write_png(filename.c_str(), size.x, size.y, (3 + alpha), pixelsStart, size.x * (3 + alpha)))
				Debug::Log("Screenshot saved as: " + filename);
		}
		else
			Debug::LogError("Could not locate or create directory \"" + std::string(folder) + "\" to store screenshots, error: " + errorCode.message() + " (" + std::to_string(errorCode.value()) + ")");
	}

#pragma region Debug Rendering TODO: move this to another class
	void RenderView::UpdateStatisticsWindow()
	{

		const char* trueFalse = "false\0true";

		// Rendering
		devBoardText.str("");
		devBoardText << "PERFORMANCE MONITOR ";
#ifdef _DEBUG
		devBoardText << "(DEBUG):";
#else
		devBoardText << "(RELEASE):";
#endif

		// Performance
		devBoardText << "\n\nMAIN PERFORMANCE ";


		double cpuTime = os::Statistics::CPUUsageByProcess();
		devBoardText.setf(std::ios::fixed, std::ios::floatfield);
		devBoardText.precision(3);
		devBoardText << "\nCPU:         ";
		devBoardText << std::right << std::setfill('·') << std::setw(7) << cpuTime;
		devBoardText << "%, " << (Diagnostics::frameTime * 1000) << "ms";
		devBoardText << "\nMEMORY PHYS: " << ((float)os::Statistics::PhysicalMemoryUsedByProcess() / 1024.f / 1024.f) << "MB";
		devBoardText << "\n       VIRT: " << ((float)os::Statistics::VirtualMemoryUsedByProcess() / 1024.f / 1024.f) << "MB";
		devBoardText << "\n        GPU: " << ((float)RenderingFactory::Instance()->GetAllocatedMemory() / 1024.f / 1024.f) << "MB";

		devBoardText << "\n\nRENDERING PERFORMANCE";
		devBoardText << "\nFPS:         " << fpsCount << " (" << (avgDeltaTime * 1000.f) << "ms)";
		if (GameEngine::IsFPSLimitEnabled())
			devBoardText << "\nFPS CAP:     true (" << targetFps << ")";
		else
			devBoardText << "\nFPS CAP:     false";

		devBoardText << "\nVSYNC:       " << (trueFalse + 6 * Settings::vSyncEnabled);
		devBoardText << "\nDRAW CALLS:  " << Diagnostics::drawCalls;

		devBoardText << "\nCPU:         " << elapsedMilliseconds << "ms";
		devBoardText << "\nGPU:         " << renderer->GetRenderTime() << "ms";
		fps->SetText(devBoardText.str());

		// Debug text
		devBoardText.str("");
		devBoardText << "WIREFRAME:     " << (trueFalse + 6 * Settings::drawLines);
		devBoardText << "\nDEBUG INFO:    " << (trueFalse + 6 * Settings::drawDebug);

		devBoardText << "\nPHYSICS:";
		devBoardText << "\n  COL. BOXES:  " << (trueFalse + 6 * Settings::drawCollisionBoxes);
		//devBoardText << "\nBONES:       " << (drawBones ? "drawn" : "hidden");

		cgc::strong_ptr<Camera> camera = world->GetRenderData()->GetCamera();
		RenderCamera* renderCamera = camera->GetRenderCameraData().ptr();

		devBoardText << "\n\nCAMERA:";
		devBoardText << "\n  POSITION:    " << (int)renderCamera->data.position.x << ", " << (int)renderCamera->data.position.y << ", " << (int)renderCamera->data.position.z;
		devBoardText << "\n  ROTATION:    " << (roundf(renderCamera->data.forward.x * 100) * 0.01f) << ", " << (roundf(renderCamera->data.forward.y * 100) * 0.01f) << ", " << ((float)(int)(renderCamera->data.forward.z * 100) * 0.01f);

		debug->SetText(devBoardText.str());
		//texthollow.setString(dev_board.str());
	}

	void RenderView::DrawStatisticsWindow()
	{
		/*fps.setFillColor(sf::Color(0, 0, 0, 191));
		fps.setPosition(15.75f, 10.75f);
		window->draw(fps);
		fps.setFillColor(sf::Color::White);
		fps.setPosition(15.f, 10.f);
		window->draw(fps);


		debug.setFillColor(sf::Color(0, 0, 0, 191));
		debug.setPosition(15.75f, screenSize.y - 119.25f);
		window->draw(debug);
		debug.setFillColor(sf::Color::White);
		debug.setPosition(15.f, screenSize.y - 120.f);
		window->draw(debug);*/

	}
#pragma endregion

	void RenderView::RenderFrame()
	{
		Diagnostics::drawCalls = 0;

		frames++;
		avgDeltaTime = Time::RenderDeltaTime();
		auto timeNow = std::chrono::high_resolution_clock::now();
		if (timeNow >= fpsCountNextTime)
		{
			fpsCount = frames;
			frames = 0;

			//while ((fpsCountNextTime += std::chrono::seconds(1)) <= timeNow); // accurate, few more clock cycles than below options
			fpsCountNextTime = timeNow + std::chrono::seconds(1); // inaccurate (adds extra micro-/nanoseconds on each iteration)
			//fpsCountNextTime = timeNow + std::chrono::seconds(1) - ((timeNow - nextUpdate) % std::chrono::seconds(1)); // removes above error to nanosecond accuracy
		}

		IRenderData* renderData = world->GetRenderData();
		cgc::raw_ptr<RenderCamera> camera = renderData->GetCamera()->GetRenderCameraData();

		// Culling
		Culling& culling = world->GetCulling();
		culling.PreRenderUpdate(camera);
		RenderList& renderList = culling.GetRenderObjectList();

		// order our list from back to front
		auto& transparentObjects = renderList[RenderObject::RenderOrder::TRANSLUCENT];
		std::sort(transparentObjects.begin(), transparentObjects.end(), [camera](RenderObject* a, RenderObject* b) -> bool
		{
			return glm::length2(a->GetPosition() - camera->data.position) > glm::length2(b->GetPosition() - camera->data.position);
		});

		// Load in queued data
		renderer->GetFactory().HandleQueue();

		// clean up
		world->DirtyRenderCleanUp();

		// 3D renderer
		renderData->SetRenderList(renderList);
		renderData->SetLights(&world->GetLights());
		renderData->GetCamera()->Update();

		renderer->RenderFrame();

		if (Settings::drawDebug)
		{
			culling.DebugRender(world->GetDebugRenderObjects());
			renderer->RenderDevObjects(world->GetDebugRenderObjects());

			// TODO: check if we can do this approach without instantiating and ditching the objects
			world->GetDebugRenderObjects().clear();
		}

		htmlContext->Render();

		// 2D sfml renderer
		window->resetGLStates();
		//htmlContext->Update();

		if (statisticsOverlay->IsVisible())
			UpdateStatisticsWindow();

		for (std::size_t i = 0; i < postRenders.size(); ++i)
			postRenders[i](window.get());

		Console::Render(window.get());

		elapsedMilliseconds = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - timeNow).count());
		
		// Show our rendered frame on the screen (swap buffers)
#ifdef _WIN32
		SwapBuffers(deviceContext);
#else
		window->display();
#endif
		//glFinish();

	}

	void RenderView::HandleEvents()
	{
		if (!windowCreated)
			return;

		IRenderData* renderData = world->GetRenderData();
		RenderCamera* camera = renderData->GetCamera()->GetRenderCameraData().ptr();

		int modifier;
		Rml::Input::KeyIdentifier rmlKey;

		sf::Event event;
		while (window->pollEvent(event))
		{

			switch (event.type)
			{
			case sf::Event::Closed:
				CloseWindow();
				break;
			case sf::Event::Resized:
				screenSize = glm::uvec2(event.size.width, event.size.height);
				screenCenterPosition = screenSize / 2;
				debug->position = glm::vec3(15.f, screenSize.y - 120.f, 0.f);

				window->SetView(sf::View(sf::FloatRect(0, 0, (float)screenSize.x, (float)screenSize.y)));
				renderer->SetScreenSize(screenSize);

				for (std::size_t i = 0; i < screenSizeListeners.size(); ++i)
					screenSizeListeners[i](screenSize);

				break;
			case sf::Event::TextEntered:
				if (event.text.unicode >= 32)
				{ 
					Input::GetKeyBoard().TextEntered(event.text.unicode);
				
					if(htmlContext->ProcessTextInput(Rml::Character(event.text.unicode)))
						if (camera->guiContext)
							camera->guiContext->ProcessTextInput(Rml::Character(event.text.unicode));
				}
				break;
			case sf::Event::KeyReleased:
				Input::GetKeyBoard().SetKeyReleased(static_cast<Keyboard::Key>(event.key.code));

				modifier = HTMLManager::GetKeyModifiers();
				rmlKey = HTMLManager::TranslateKey(event.key.code);
				if (htmlContext->ProcessKeyUp(rmlKey, modifier))
					if (camera->guiContext)
						camera->guiContext->ProcessKeyUp(rmlKey, modifier);

				switch (event.key.code)
				{
#ifndef __APPLE__
				case sf::Keyboard::F11:
					if (Input::GetKeyBoard().IsKeyDown(Keyboard::RAlt))
#else
				case sf::Keyboard::F10:
					if (Input::GetKeyBoard().IsKeyDown(Keyboard::LSystem))
#endif
					{
						if (statisticsOverlay->GetRenderCamera() != world->GetRenderCamera()->GetRenderCameraData())
						{
							bool wasLoaded = statisticsOverlay->GetRenderCamera();
							world->GetRenderCamera()->AddGraphicOverlay(statisticsOverlay);

							if (!wasLoaded)
							{
								statisticsOverlay->AddGraphicObject(fps);
								statisticsOverlay->AddGraphicObject(debug);
							}
						}

						statisticsOverlay->SetVisible(!statisticsOverlay->IsVisible());
					}
					break;
				case sf::Keyboard::F12:
					ScreenShot();
					break;
#ifdef __APPLE__
				case sf::Keyboard::R:
					if (Input::GetKeyBoard().IsKeyDown(Keyboard::LSystem))
					{
						if (Console::IsOpen())
							Console::Close(world->GetRenderCamera());
						else
							Console::Open(world->GetRenderCamera());
					}
					break;
				case 56: // Tilde on mac?
#else
				case sf::Keyboard::Tilde:
#endif
					if (!Input::GetKeyBoard().IsKeyDown(Keyboard::LShift) && !Input::GetKeyBoard().IsKeyDown(Keyboard::RShift))					
					{
						if (Console::IsOpen())
							Console::Close(world->GetRenderCamera());
						else
							Console::Open(world->GetRenderCamera());
					}
					break;
				};
				break;
			case sf::Event::KeyPressed:
				Input::GetKeyBoard().SetKeyPressed(reinterpret_cast<Keyboard::Key&>(event.key.code));

				modifier = HTMLManager::GetKeyModifiers();
				rmlKey = HTMLManager::TranslateKey(event.key.code);
				if (htmlContext->ProcessKeyDown(rmlKey, modifier))
					if (camera->guiContext)
						camera->guiContext->ProcessKeyDown(rmlKey, modifier);
				break;
			case sf::Event::MouseButtonPressed:
			{
				Input::GetMouse().SetButtonPressed(reinterpret_cast<Mouse::Button&>(event.mouseButton.button));

				modifier = HTMLManager::GetKeyModifiers();
				if (htmlContext->ProcessMouseButtonDown(event.mouseButton.button, modifier))
					if (camera->guiContext)
						camera->guiContext->ProcessMouseButtonDown(event.mouseButton.button, modifier);
			}
			break;
			case sf::Event::MouseButtonReleased:
				Input::GetMouse().SetButtonReleased(reinterpret_cast<Mouse::Button&>(event.mouseButton.button));

				modifier = HTMLManager::GetKeyModifiers();
				if (htmlContext->ProcessMouseButtonUp(event.mouseButton.button, modifier))
					if (camera->guiContext)
						camera->guiContext->ProcessMouseButtonUp(event.mouseButton.button, modifier);
				break;
			case sf::Event::MouseMoved:
				modifier = HTMLManager::GetKeyModifiers();
				if (htmlContext->ProcessMouseMove(event.mouseMove.x, event.mouseMove.y, modifier))
					if(camera->guiContext)
						camera->guiContext->ProcessMouseMove(event.mouseMove.x, event.mouseMove.y, modifier);
				break;
			case sf::Event::MouseWheelMoved:
				Input::GetMouse().SetWheelDelta(event.mouseWheel.delta);

				modifier = HTMLManager::GetKeyModifiers();
				if (htmlContext->ProcessMouseWheel(float(-event.mouseWheel.delta), modifier))
					if (camera->guiContext)
						camera->guiContext->ProcessMouseWheel(float(-event.mouseWheel.delta), modifier);
				break;
			/*case sf::Event::LostFocus: // unreliable
				screenFocused = false;
				Input::Disable();
				break;
			case sf::Event::GainedFocus:
				screenFocused = true;
				Input::Enable();
				break;*/
			}
		}

		htmlContext->Update();
		if (camera->guiContext)
			camera->guiContext->Update();

		bool hasFocus = window->hasFocus();
		if (screenFocused != hasFocus)
		{
			screenFocused = hasFocus;
			if (screenFocused)
				Input::Enable();
			else
				Input::Disable();
		}
	}

	void RenderView::RegisterScreenSizeListener(const ScreenSizedelegate& function)
	{
		screenSizeListeners.emplace_back(function);
	}

	void RenderView::UnRegisterScreenSizeListener(const ScreenSizedelegate& function)
	{
		auto found = std::find(screenSizeListeners.begin(), screenSizeListeners.end(), function);
		if (found != screenSizeListeners.end())
			screenSizeListeners.erase(found);
	}

	void RenderView::SetFPSLimit(uint limit)
	{
		window->setFramerateLimit(limit);
	}

	void RenderView::UpdateScreenRectangle()
	{
#ifdef _WIN32
		GetClientRect(systemHandle, &windowRect);
		ClientToScreen(systemHandle, reinterpret_cast<tagPOINT*>(&windowRect.left));
		ClientToScreen(systemHandle, reinterpret_cast<tagPOINT*>(&windowRect.right));
#else
		//Debug::Log("RenderView::UpdateScreenRectangle(): Not implemented");
#endif
	}

	void RenderView::LockCursorInScreen(bool lock)
	{

#ifdef _WIN32
		UpdateScreenRectangle();
		ClipCursor(lock ? &windowRect : nullptr);
#else
		window->setMouseCursorGrabbed(lock);
#endif
	}

	void RenderView::EnableStatisticsWindow(bool enable)
	{
		statisticsOverlay->SetVisible(enable);
	}

	void RenderView::OnCommand(const std::string& command, const std::string& value)
	{
		hash_t valueHash = RT_HASH(value);

		switch (RT_HASH(command))
		{
		case CT_HASH("cl_vsync"):
			if (valueHash == CT_HASH("on") || valueHash == CT_HASH("true"))
			{
				Settings::vSyncEnabled = true;
				window->setVerticalSyncEnabled(true);
				if (GameEngine::IsFPSLimitEnabled())
					GameEngine::EnableFPSLimit(false);
			}
			else if (valueHash == CT_HASH("off") || valueHash == CT_HASH("false"))
			{
				Settings::vSyncEnabled = false;
				window->setVerticalSyncEnabled(false);
			}
			else
				Console::WriteLine("cl_vsync on|off|true|false");
			break;
		case CT_HASH("cl_fpscap"):
			if (valueHash == CT_HASH("on") || valueHash == CT_HASH("true"))
				GameEngine::EnableFPSLimit(true);
			else if (valueHash == CT_HASH("off") || valueHash == CT_HASH("false"))
				GameEngine::EnableFPSLimit(false);
			else
			{
				char* p;
				long converted = strtol(value.c_str(), &p, 10);
				if (*p)
					GameEngine::SetTargetFPS(converted);
				else
					Console::WriteLine("cl_fpscap on|off|true|false|30|60|number");
			}
			break;
		case CT_HASH("cl_wireframe"):
			if (valueHash == CT_HASH("on") || valueHash == CT_HASH("true"))
				Settings::drawLines = true;
			else if (valueHash == CT_HASH("off") || valueHash == CT_HASH("false"))
				Settings::drawLines = false;
			else
				Console::WriteLine("cl_wireframe on|off|true|false");
			break;
		case CT_HASH("cl_draw_debug"):
			if (valueHash == CT_HASH("on") || valueHash == CT_HASH("true"))
				Settings::drawDebug = true;
			else if (valueHash == CT_HASH("off") || valueHash == CT_HASH("false"))
				Settings::drawDebug = false;
			else
				Console::WriteLine("cl_draw_debug on|off|true|false");
			break;
		case CT_HASH("cl_draw_collision_boxes"):
			if (valueHash == CT_HASH("on") || valueHash == CT_HASH("true"))
				Settings::drawCollisionBoxes = true;
			else if (valueHash == CT_HASH("off") || valueHash == CT_HASH("false"))
				Settings::drawCollisionBoxes = false;
			else
				Console::WriteLine("cl_draw_collision_boxes on|off|true|false");
			break;
		case CT_HASH("cl_refresh_hud"):
			/*Rml::ElementDocument* doc = htmlDocument;
			htmlContext->UnloadDocument(doc);

			htmlDocument = htmlContext->LoadDocument("./resources/gui/menu.html");
			if (htmlDocument)
			{
				htmlDocument->Show();
				Debug::Log("Document loaded");
			}
			else
			{
				Debug::Log("Document is not loaded");
			}

			htmlContext->Update();*/

			//delete doc;
			break;
		}
	}
	
	void RenderView::RegisterPostRender(PostRenderdelegate postRender)
	{
		postRenders.emplace_back(postRender);
	}

	void RenderView::UnRegisterPostRender(PostRenderdelegate postRender)
	{
		auto found = std::find(postRenders.begin(), postRenders.end(), postRender);
		if (found != postRenders.end())
			postRenders.erase(found);
	}
}