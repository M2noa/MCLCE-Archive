#pragma once

#include "stdafx.h"
#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>

namespace Esteem
{
	class Window : public sf::RenderWindow
	{
	private:
		sf::Uint32 style;
		glm::uvec2 windowedSize;
		glm::uvec2 fullscreenSize;

	public:
		Window();
		Window(sf::VideoMode mode, const sf::String& title, sf::Uint32 style = sf::Style::Default, const sf::ContextSettings& settings = sf::ContextSettings());
		Window(sf::WindowHandle handle, const sf::ContextSettings& settings = sf::ContextSettings());
		~Window();

		void SetStyle(sf::Uint32 style);
		void FullScreen(bool fullscreen = true);

		inline bool IsFullScreen() { return (style & sf::Style::Fullscreen) > 0; }

		void SetSize(const glm::uvec2& size);
		void SetView(const sf::View& view);
	};
}