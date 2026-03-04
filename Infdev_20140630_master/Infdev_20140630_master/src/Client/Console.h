#pragma once

#include <string>
#include <vector>
#include <sstream>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <SFML/Graphics.hpp>

//#include "Input/Keyboard.h"
//#include "Rendering/Objects/Text.h"
//#include "Utils/details/sfml_stream.h"

#include <World/Constituents/Camera.h>

#include <RmlUi/Core/EventListener.h>
#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Core/Elements/ElementFormControlTextArea.h>
#include <RmlUi/Core/Elements/ElementFormControlInput.h>
#include <RmlUi/Core/Elements/ElementFormControlDataSelect.h>

namespace Esteem
{
	class InputFieldListener : public Rml::EventListener
	{
	public:
		InputFieldListener()
		{ }

		void ProcessEvent(Rml::Event& event) override;
	};

	class Console
	{
	private:
		static bool opened;
		static bool commandLineFocused;
		static bool prevCursorVisible;
		static bool prevCursorLocked;
		static float charSpotTimer;

		static size_t maxCommandHistorySize;

		static std::string tempCommandLine;
		static std::string commandLine;
		static std::stringstream consoleLines;
		static std::vector<std::string> commandHistory;
		
		static uint consoleLinesCount;
		static std::size_t historyIndex;

		// 2D drawing
		/*static glm::vec4 rect;
		static cgc::strong_ptr<GraphicalOverlay> consoleOverlay;

		static cgc::strong_ptr<Esteem::Text> textCommandLine;
		static cgc::strong_ptr<Esteem::Text> textConsoleLines;
		static cgc::strong_ptr<Esteem::Text> textSuggestions;
		
		static cgc::strong_ptr<Esteem::GraphicalOverlay::OverlayQuad> quadBackground;
		static cgc::strong_ptr<Esteem::GraphicalOverlay::OverlayQuad> quadConsoleLinesBackground;
		static cgc::strong_ptr<Esteem::GraphicalOverlay::OverlayQuad> quadCommandLineBackground;
		static cgc::strong_ptr<Esteem::GraphicalOverlay::OverlayQuad> quadSuggestionsBackground;*/

		static Rml::ElementDocument* htmlDocument;
		static Rml::ElementFormControlTextArea* consoleScreen;
		static Rml::ElementFormControlInput* consoleInput;
		static Rml::Element* consoleSuggestions;

		static std::vector<std::string> commands;
		static std::vector<std::pair<int, const std::string*>> suggestions;
		static std::size_t selectedSuggestionIndex;
		static std::chrono::microseconds lastSuggestionsCheck;
		static bool lastCommandChangeSuggestionsChecked;

		static InputFieldListener eventListener;

		static void AddSuggestion(const std::string& command);
		static void CheckSuggestions(const std::string& command);

	public:
		static void Initialize(sf::Window* window, Rml::Context* context);
		static void Destroy();

		inline static bool IsOpen() { return opened; }

		static void Open(cgc::raw_ptr<Camera> world);
		static void Close(cgc::raw_ptr<Camera> world);

		static void Render(sf::RenderWindow* window);
		
		static bool OnTextEntered(uint32 character);
		static bool WriteLine(const std::string& line);

		template <typename ...Ts>
		inline static bool WriteLine(const Ts... lines)
		{
			//if (lines.size() > 0)
			{
				((consoleLines << lines), ...);
				consoleLines << "\n";

				if (IsOpen())
					consoleScreen->SetValue(consoleLines.str());

				return true;
			}

			return false;
		}
	};
}