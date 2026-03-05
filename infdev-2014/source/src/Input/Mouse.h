#pragma once

#include "stdafx.h"
#include <glm/vec2.hpp>
#include <SFML/Window/Mouse.hpp>

#include "./IInputSystem.h"
#include "Window/IView.h" // TODO: do we really need IView?
#include "Window/Window.h"


#ifdef _WIN32
#	include <windows.h>
#endif

namespace Esteem
{
	/// \brief Not yet multi-view supported
	///
	///
	///
	class Mouse : public IInputSystem
	{
		friend class RenderView;
	public:
		enum Button
		{
			Left,       ///< The left mouse button
			Right,      ///< The right mouse button
			Middle,     ///< The middle (wheel) mouse button
			XButton1,   ///< The first extra mouse button
			XButton2,   ///< The second extra mouse button

			ButtonCount ///< Keep last -- the total number of mouse buttons
		};

		enum Wheel
		{
			VerticalWheel,  ///< The vertical mouse wheel
			HorizontalWheel ///< The horizontal mouse wheel
		};

	private:
		byte buttonStates[Button::ButtonCount];
		glm::vec2 deltaMove;
		glm::ivec2 prevMousePosition;
		glm::ivec2 wheelDelta;

		bool lockCursor;
		IView* view;
		Window* window;

	#ifdef _WIN32
		tagPOINT cursorPosition;
	#endif


		inline void SetLockCursor(bool lock) { this->lockCursor = lock; }

	public:
		Mouse();

		virtual void Update();

		void SetView(IView* view, Window* window);

		void Enable();
		void Disable();

		void LockCursor(bool lock);
		inline bool IsCursorLocked() { return lockCursor; }

		inline void SetCursorVisible(bool visible) { view->SetMouseCursorVisible(visible); }
		inline bool IsCursorVisible() { return view->IsMouseCursorVisible(); }
		
		inline const glm::vec2 GetDeltaMove() { return deltaMove; }

		inline const glm::ivec2 GetPosition() { sf::Vector2i mPos = sf::Mouse::getPosition(*window); return reinterpret_cast<glm::ivec2&>(mPos); }
		//inline const glm::ivec2 GetPosition(const sf::Window& window) { return reinterpret_cast<glm::ivec2&>(sf::Mouse::getPosition(window)); }

		inline void SetPosition(glm::ivec2& position) { sf::Mouse::setPosition(reinterpret_cast<sf::Vector2i&>(position), *window); }
		//inline void SetPosition(glm::ivec2& position, const sf::Window& window) { sf::Mouse::setPosition(reinterpret_cast<sf::Vector2i&>(position), window); }


		inline void SetWheelDelta(int delta) { wheelDelta.x = delta; }
		inline int GetWheelDelta() { return wheelDelta.x; }

		inline void SetButtonReleased(Button key) { buttonStates[key] = 1; }
		inline void SetButtonPressed(Button key) { buttonStates[key] = 3; }

		inline bool ButtonPressed(Button key) { return buttonStates[key] == 3; }
		inline bool ButtonReleased(Button key) { return buttonStates[key] == 1; }
		inline bool IsButtonDown(Button key) { return buttonStates[key] >= 2; }
		inline bool IsButtonUp(Button key) { return buttonStates[key] <= 1; }
	};
}