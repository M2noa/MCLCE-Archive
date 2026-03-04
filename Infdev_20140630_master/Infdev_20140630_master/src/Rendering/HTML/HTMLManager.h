#pragma once

#include <RmlUi/Core.h>
#include <RmlUi/Core/Input.h>
#include <RmlUi/Core/SystemInterface.h>
#include <SFML/Window.hpp>

namespace Esteem
{
	class HTMLManager : public Rml::SystemInterface
	{
	private:
		sf::Clock timer;

	public:
		static Rml::Input::KeyIdentifier TranslateKey(sf::Keyboard::Key Key);
		static int GetKeyModifiers();

		double GetElapsedTime() override;
		bool LogMessage(Rml::Log::Type type, const std::string& message) override;

		/// Set clipboard text.
		/// @param[in] text Text to apply to clipboard.
		void SetClipboardText(const std::string& text) override;

		/// Get clipboard text.
		/// @param[out] text Retrieved text from clipboard.
		void GetClipboardText(std::string& text) override;
	};
}