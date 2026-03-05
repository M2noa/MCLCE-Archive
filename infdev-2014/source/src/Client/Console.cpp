#include "Console.h"

#include "Utils/Time.h"
#include "Utils/Data.h"
#include "Utils/Debug.h"
#include "Utils/StringParser.h"
#include "Utils/details/sfml_stream.h"
#include "General/Command.h"
#include "Input/Input.h"
#include "Window/View.h"
#include "Rendering/Objects/Text.h"
#include "World/Constituents/Camera.h"

#include <cppu/logic/LevenshteinDistance.h>

#include <RmlUi/Core/Elements/InputTypeText.h>
#include <RmlUi/Core/Elements/WidgetTextInput.h>


namespace Esteem
{
	bool Console::opened = false;
	bool Console::commandLineFocused = false;
	bool Console::prevCursorVisible = false;
	bool Console::prevCursorLocked = false;
	float Console::charSpotTimer = 0;

	size_t Console::maxCommandHistorySize = 20;

	//glm::vec4 Console::rect = glm::vec4(0, 0, 1280, 720);

	std::string Console::tempCommandLine;
	std::string Console::commandLine;
	std::stringstream Console::consoleLines;
	std::vector<std::string> Console::commandHistory;

	uint Console::consoleLinesCount = 0;
	std::size_t Console::historyIndex = -1;

	InputFieldListener Console::eventListener;

	// 2D drawing
	/*cgc::strong_ptr<GraphicalOverlay> Console::consoleOverlay;

	cgc::strong_ptr<Esteem::Text> Console::textCommandLine;
	cgc::strong_ptr<Esteem::Text> Console::textConsoleLines;
	cgc::strong_ptr<Esteem::Text> Console::textSuggestions;

	cgc::strong_ptr<GraphicalOverlay::OverlayQuad> Console::quadBackground;
	cgc::strong_ptr<GraphicalOverlay::OverlayQuad> Console::quadConsoleLinesBackground;
	cgc::strong_ptr<GraphicalOverlay::OverlayQuad> Console::quadCommandLineBackground;
	cgc::strong_ptr<Esteem::GraphicalOverlay::OverlayQuad> Console::quadSuggestionsBackground;*/


	Rml::ElementDocument* Console::htmlDocument = nullptr;
	Rml::ElementFormControlTextArea* Console::consoleScreen = nullptr;;
	Rml::ElementFormControlInput* Console::consoleInput = nullptr;;
	Rml::Element* Console::consoleSuggestions = nullptr;

	std::vector<std::string> Console::commands;
	std::vector<std::pair<int, const std::string*>> Console::suggestions;
	std::size_t Console::selectedSuggestionIndex = 0;
	std::chrono::microseconds Console::lastSuggestionsCheck = {};
	bool Console::lastCommandChangeSuggestionsChecked = true;

	void Console::Initialize(sf::Window* window, Rml::Context* context)
	{
		/*consoleOverlay = cgc::construct_new<GraphicalOverlay>("Console");
		consoleOverlay->SetOrder(std::numeric_limits<int>::max());

		cgc::strong_ptr<Material> quadMaterial = RenderingFactory::Instance()->LoadMaterial("GUI/default_color");
		quadBackground = cgc::construct_new<GraphicalOverlay::OverlayQuad>(quadMaterial);
		quadBackground->color = Color8u(0, 0, 0, 128u);

		quadCommandLineBackground = cgc::construct_new<GraphicalOverlay::OverlayQuad>(quadMaterial);
		quadCommandLineBackground->color = Color8u(0, 0, 0, 128u);

		quadConsoleLinesBackground = cgc::construct_new<GraphicalOverlay::OverlayQuad>(quadMaterial);
		quadConsoleLinesBackground->color = Color8u(0, 0, 0, 128u);

		quadSuggestionsBackground = cgc::construct_new<GraphicalOverlay::OverlayQuad>(quadMaterial);
		quadSuggestionsBackground->color = Color8u(50u, 50u, 50u, 128u);

		cgc::strong_ptr<Font> font = Font::LoadFont("resources/fonts/consolas.ttf");

		textCommandLine = cgc::gcnew<Esteem::Text>();
		textCommandLine->SetFont(font);
		textCommandLine->SetFontSize(12);

		textConsoleLines = cgc::gcnew<Esteem::Text>();
		textConsoleLines->SetFont(font);
		textConsoleLines->SetFontSize(12);
		textConsoleLines->alignment = Text::LEFT_BOTTOM;

		textSuggestions = cgc::gcnew<Esteem::Text>();
		textSuggestions->SetColor(Color8u(200, 200, 200));
		textSuggestions->SetFont(font);
		textSuggestions->SetFontSize(12);

		sf::Vector2u wSize = window->getSize();
		Console::SetRect(rect, (glm::vec2)reinterpret_cast<glm::uvec2&>(wSize));*/

		Command::RegisterListener(DELEGATE(&Console::AddSuggestion));

		const std::string consoleURL = "resources/gui/console.html";
		if (Esteem::Data::AssetExists(consoleURL))
		{
			htmlDocument = context->LoadDocumentFromMemory(Esteem::Data::ReadAsset(consoleURL), consoleURL);
			if (htmlDocument)
			{
				htmlDocument->Hide();

				consoleScreen = static_cast<Rml::ElementFormControlTextArea*>(htmlDocument->GetElementById("console-screen"));

				consoleInput = static_cast<Rml::ElementFormControlInput*>(htmlDocument->GetElementById("console-input"));
				consoleInput->AddEventListener(Rml::EventId::Keydown, &eventListener);

				consoleSuggestions = htmlDocument->GetElementById("console-suggestions");
				consoleSuggestions->SetProperty(Rml::PropertyId::Display, Rml::Property(Rml::Style::Display::None));

				context->Update();

				return;
			}
		}
		
		Debug::LogError("Couldn't load console");
	}

	void Console::Destroy()
	{
		//consoleInput->
	}

	void Console::Open(cgc::raw_ptr<Camera> camera)
	{
		prevCursorVisible = Input::GetMouse().IsCursorVisible();
		prevCursorLocked = Input::GetMouse().IsCursorLocked();
		Input::GetMouse().SetCursorVisible(true);
		Input::GetMouse().LockCursor(false);
		opened = true;
		commandLineFocused = true;

		consoleScreen->SetValue(consoleLines.str());
		htmlDocument->Show();
		consoleInput->Focus();
		//consoleOverlay->SetVisible(true);

	}

	void Console::Close(cgc::raw_ptr<Camera> camera)
	{
		if(htmlDocument->GetFocusLeafNode())
			htmlDocument->GetFocusLeafNode()->Blur();

		htmlDocument->Blur();
		htmlDocument->Hide();
		//consoleOverlay->SetVisible(false);

		//Input::GetKeyBoard().UnRegisterTextEntered(DELEGATE(&Console::OnTextEntered));
		Input::GetMouse().SetCursorVisible(prevCursorVisible);
		Input::GetMouse().LockCursor(prevCursorLocked);
		opened = false;
		commandLineFocused = false;
	}

	void Console::Render(sf::RenderWindow* window)
	{
		if (opened)
		{
			if (!lastCommandChangeSuggestionsChecked && lastSuggestionsCheck + std::chrono::milliseconds(1000) < Time::RenderTime())
			{
				lastCommandChangeSuggestionsChecked = true;
				lastSuggestionsCheck = Time::RenderTime();
				CheckSuggestions(consoleInput->GetValue());
			}
		}
	}

	bool Console::OnTextEntered(uint32 character)
	{
		if (htmlDocument->GetFocusLeafNode() == consoleInput)
		{
			switch (character)
			{
			case Rml::Input::KI_RETURN: // carriage return key
			//case 13: // carriage return key
			{
				std::string commandLine = consoleInput->GetValue();
				if (!commandLine.empty())
				{
					WriteLine(commandLine);

					if (commandHistory.size() > maxCommandHistorySize)
						commandHistory.erase(commandHistory.begin());

					commandHistory.emplace_back(commandLine);
					historyIndex = commandHistory.size();

					std::string command;
					std::string value;

					int space = commandLine.find(" ", 0);
					if (space != commandLine.npos)
					{
						command = commandLine.substr(0, space);
						value = commandLine.substr(space + 1);
						StringParser::Trim(value);
					}
					else
						command = commandLine;

					Command::ExecuteCommand(command, value);
				}
				suggestions.clear();
				consoleInput->SetValue("");
				consoleSuggestions->SetProperty(Rml::PropertyId::Display, Rml::Property(Rml::Style::Display::None));
				lastCommandChangeSuggestionsChecked = false;
				break;
			}
			case Rml::Input::KI_UP:
			{
				if (suggestions.size() > 1)
				{
					if (selectedSuggestionIndex > 0)
						selectedSuggestionIndex--;
					
					consoleInput->SetValue(*suggestions[selectedSuggestionIndex].second + (selectedSuggestionIndex == 0 ? "" : " "));
				}
				else if (!commandHistory.empty())
				{
					if (historyIndex > 0)
						historyIndex--;

					consoleInput->SetValue(commandHistory[historyIndex]);
				}

				static_cast<Rml::InputTypeText*>(consoleInput->GetInputType())->GetWidgetTextInput()->MoveCursorHorizontal(Rml::WidgetTextInput::CursorMovement::EndLine);
				htmlDocument->UpdateDocument();

				lastCommandChangeSuggestionsChecked = true;

				return false; // don't do anything else with the up/down key event
				break;
			}
			case Rml::Input::KI_DOWN:
			{
				if (suggestions.size() > 1)
				{
					if (selectedSuggestionIndex < suggestions.size() - 1)
						selectedSuggestionIndex++;
					
					consoleInput->SetValue(*suggestions[selectedSuggestionIndex].second + " ");
				}
				else if (!commandHistory.empty())
				{
					if (historyIndex < commandHistory.size() - 1)
						historyIndex++;
					
					if(historyIndex < commandHistory.size())
						consoleInput->SetValue(commandHistory[historyIndex]);
				}

				static_cast<Rml::InputTypeText*>(consoleInput->GetInputType())->GetWidgetTextInput()->MoveCursorHorizontal(Rml::WidgetTextInput::CursorMovement::EndLine);
				htmlDocument->UpdateDocument();

				lastCommandChangeSuggestionsChecked = true;

				return false; // don't do anything else with the up/down key event
				break;
			}
			default:
				historyIndex = commandHistory.size();
				lastCommandChangeSuggestionsChecked = false;
			}
		}

		return true;
	}

	bool Console::WriteLine(const std::string& line)
	{
		//if (!line.empty())
		{
			consoleLines << line << '\n';

			if (IsOpen())
			{
				consoleScreen->SetValue(consoleLines.str());
				htmlDocument->UpdateDocument();
			}

			return true;
		}

		return false;
	}

	void Console::AddSuggestion(const std::string& command)
	{
		commands.emplace_back(command);
	}

	void Console::CheckSuggestions(const std::string& command)
	{
		suggestions.clear();

		if (!command.empty())
		{
			tempCommandLine = command;

			const std::size_t MAX_SUGGESTIONS = 10;
			std::size_t count = 0;
			suggestions.emplace_back(0, &tempCommandLine);

			std::string::const_iterator begin = command.begin();
			for (; begin != command.end() && *begin == ' '; ++begin);

			std::string::const_iterator end = begin;
			for (; end != command.end() && *end != ' '; ++end);

			for (auto& com : commands)
			{
				std::size_t cost = logic::LevenshteinDistance(begin, end, com.begin(), com.end(), 0, 1, 2);
				if (cost <= 0)
				{
					suggestions.emplace_back(cost, &com);

					if (++count > MAX_SUGGESTIONS)
						break;
				}
			}

			std::sort(suggestions.begin() + 1, suggestions.end(),
				[](const std::pair<int, const std::string*>& left, const std::pair<int, const std::string*>& right) { return left.first < right.first; });

			std::string suggestionsText = "";
			for (auto suggestion = suggestions.begin() + 1; suggestion != suggestions.end(); ++suggestion)
			{
				suggestionsText += *suggestion->second + "<br />";
				//consoleSuggestions->Add(*suggestion->second, *suggestion->second);
			}

			if (!suggestions.empty())
			{
				consoleSuggestions->SetInnerRML(suggestionsText);
				consoleSuggestions->RemoveProperty(Rml::PropertyId::Display);
				consoleSuggestions->GetOwnerDocument()->UpdateDocument();
			}
			else
				consoleSuggestions->SetProperty(Rml::PropertyId::Display, Rml::Property(Rml::Style::Display::None));
		}
		else
			consoleSuggestions->SetProperty(Rml::PropertyId::Display, Rml::Property(Rml::Style::Display::None));

		selectedSuggestionIndex = 0;
	}

	void InputFieldListener::ProcessEvent(Rml::Event& event)
	{
		if(!Console::OnTextEntered(event.GetParameter<int>("key_identifier", 0)))
			event.StopImmediatePropagation();
	}
}