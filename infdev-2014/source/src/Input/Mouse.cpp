#include "./Mouse.h"
#include "Utils/Debug.h"

#ifdef __APPLE__
#	include <CoreGraphics/CoreGraphics.h>
#endif

namespace Esteem
{
	Mouse::Mouse()
		: lockCursor(false)
		, view(nullptr)
		, window(nullptr)
		, deltaMove(0)
		, prevMousePosition(0)
		, wheelDelta(0)
	{ }

	void Mouse::Update()
	{
		for (uint i = 0; i < sizeof(buttonStates); ++i)
		{
			if (buttonStates[i] & ~0x2)
				--buttonStates[i];
		}

		wheelDelta.x = 0;

#ifdef _WIN32
		sf::Vector2i mPos = sf::Mouse::getPosition(*window);
		glm::ivec2 mousePosition(reinterpret_cast<glm::ivec2&>(mPos));
		deltaMove = mousePosition - prevMousePosition;

		if (lockCursor)
		{
			if (deltaMove.x != 0 || deltaMove.y != 0)
			{
				prevMousePosition = view->GetScreenCenterPosition();
				sf::Mouse::setPosition(reinterpret_cast<sf::Vector2i&>(prevMousePosition), *window);
			}
		}
		else
			prevMousePosition = mousePosition;
#elif defined(__APPLE__)
		glm::ivec2 delta;
		CGGetLastMouseDelta(static_cast<int32_t*>(&delta.x), static_cast<int32_t*>(&delta.y));
		deltaMove = delta;
#endif
	}

	void Mouse::SetView(IView* view, Window* window)
	{
		this->view = view;
		this->window = window;

		if (lockCursor)
			prevMousePosition = view->GetScreenCenterPosition();
		else
		{
			sf::Vector2i mPos = sf::Mouse::getPosition(*window);
			prevMousePosition = reinterpret_cast<glm::ivec2&>(mPos);
		}
	}

	void Mouse::LockCursor(bool lock)
	{
		lockCursor = lock;
		view->LockCursorInScreen(lock);
	}
	
	void Mouse::Enable()
	{
		if(lockCursor)
			view->LockCursorInScreen(true);

		if (!IsCursorVisible())
			window->setMouseCursorVisible(false);
	}

	void Mouse::Disable()
	{
		prevMousePosition = view->GetScreenCenterPosition();
		deltaMove = glm::vec2(0);

		if (lockCursor)
			view->LockCursorInScreen(false);

		if (!IsCursorVisible())
			window->setMouseCursorVisible(true); // don't store new value, so set directly
	}
}