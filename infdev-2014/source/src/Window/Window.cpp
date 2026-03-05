#include "Window.h"

#include "General/Settings.h"
#include "Utils/Debug.h"

#ifdef _WIN32
#	include <windows.h>
#endif

namespace Esteem
{
	Window::Window()
		: sf::RenderWindow()
	{

	}

	Window::Window(sf::VideoMode mode, const sf::String& title, sf::Uint32 style, const sf::ContextSettings& settings)
		: sf::RenderWindow(mode, title, style, settings),
		style(style)
	{
		if (style & sf::Style::Fullscreen)
		{
			fullscreenSize = glm::uvec2(mode.width, mode.height);
			windowedSize = Settings::initialScreenSize;
		}
		else
		{
			windowedSize = glm::uvec2(mode.width, mode.height);
			const auto& mode = sf::VideoMode().getDesktopMode();
			fullscreenSize = glm::uvec2(mode.width, mode.height);
		}
	}

	Window::Window(sf::WindowHandle handle, const sf::ContextSettings& settings)
		: sf::RenderWindow(handle, settings),
		style(style)
	{

	}


	Window::~Window()
	{
	}

	void Window::SetStyle(sf::Uint32 style)
	{
		this->style = style;

#ifdef _WIN32
		HWND handle = this->getSystemHandle();
		DWORD win32Style = WS_VISIBLE;

		if (style == sf::Style::None)
			win32Style |= WS_POPUP;
		else
		{
			if (style & sf::Style::Titlebar)
				win32Style |= WS_CAPTION | WS_MINIMIZEBOX;

			if (style & sf::Style::Resize)
				win32Style |= WS_THICKFRAME | WS_MAXIMIZEBOX;

			if (style & sf::Style::Close)
				win32Style |= WS_SYSMENU;

			//if (style & sf::Style::Fullscreen)
			//	win32Style |= WS_EX_TOPMOST;
		}

		SetWindowLongPtr(handle, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_TOPMOST);
		SetWindowLongPtr(handle, GWL_STYLE, win32Style);

		// Force changes to take effect
		SetWindowPos(handle, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);

		//this->set
#else
		Debug::Log("Window::SetStyle(): Currently not implemented");
#endif
	}

	void Window::FullScreen(bool fullscreen)
	{
		if (fullscreen)
			style |= sf::Style::Fullscreen;
		else
			style &= ~sf::Style::Fullscreen;

#ifdef _WIN32
		HWND handle = this->getSystemHandle();
		bool isChangeSuccessful;

		if (fullscreen)
		{
			DEVMODE fullscreenSettings;
			EnumDisplaySettings(NULL, 0, &fullscreenSettings);
			fullscreenSettings.dmPelsWidth = fullscreenSize.x;
			fullscreenSettings.dmPelsHeight = fullscreenSize.y;
			fullscreenSettings.dmBitsPerPel = this->getSettings().depthBits;
			//fullscreenSettings.dmDisplayFrequency = 60;
			fullscreenSettings.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL /*| DM_DISPLAYFREQUENCY*/;

			SetWindowLongPtr(handle, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_TOPMOST);
			SetWindowLongPtr(handle, GWL_STYLE, WS_POPUP | WS_VISIBLE);
			SetWindowPos(handle, HWND_NOTOPMOST, 0, 0, fullscreenSize.x, fullscreenSize.y, SWP_SHOWWINDOW);
			isChangeSuccessful = ChangeDisplaySettings(&fullscreenSettings, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL;
			//ShowWindow(handle, SW_MAXIMIZE);

			this->setSize(sf::Vector2u(fullscreenSize.x, fullscreenSize.y));
		}
		else
		{
			int consolePosX, consolePosY;
			RECT rectScreen;

			HWND hwndScreen = GetDesktopWindow();
			GetWindowRect(hwndScreen, &rectScreen);
			consolePosX = ((rectScreen.right - rectScreen.left) / 2 - windowedSize.x / 2);
			consolePosY = ((rectScreen.bottom - rectScreen.top) / 2 - windowedSize.y / 2);

			// size with border padding (currently post resetting it with sfml)
			SetWindowLongPtr(handle, GWL_EXSTYLE, WS_EX_LEFT);
			SetWindowLongPtr(handle, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
			isChangeSuccessful = ChangeDisplaySettings(NULL, CDS_RESET) == DISP_CHANGE_SUCCESSFUL;
			SetWindowPos(handle, HWND_NOTOPMOST, consolePosX, consolePosY, windowedSize.x, windowedSize.y, SWP_SHOWWINDOW);
			//ShowWindow(handle, SW_RESTORE);

			this->setSize(sf::Vector2u(windowedSize.x, windowedSize.y));
		}
#else
		Debug::Log("Window::FullScreen(): Currently not implemented");
#endif
	}

	void Window::SetSize(const glm::uvec2& size)
	{
		if (this->IsFullScreen())
			fullscreenSize = size;
		else
			windowedSize = size;
	}

	void Window::SetView(const sf::View& view)
	{
		SetSize(glm::uvec2(view.getSize().x, view.getSize().y));
		setView(view);
	}
}