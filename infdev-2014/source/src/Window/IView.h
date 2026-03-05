#pragma once

#include "General/Delegate.h"

//#include "World/WorldController.h"
#include "Rendering/Objects/RenderCamera.h"
#include "Rendering/RenderingFactory.h"

namespace Esteem
{
	class World;

	/// \brief Interface for the View class
	/// 
	/// \author Y. Schaeffer
	/// \date March 2015
	class IView
	{
	public:
		typedef delegate<void(const glm::ivec2&)> ScreenSizedelegate;

		/// \brief Destructor
		virtual ~IView() = default;

		/// \brief handle events and window specific commands
		virtual void WindowUpdate() = 0;
		/// \brief advance 1 frame
		virtual void Update() = 0;
		/// \brief render frame
		virtual void Render() = 0;
		/// \brief set the world that we will render
		virtual void SetWorld(World* world) = 0;
		
		virtual bool IsScreenFocused() = 0;
		
		/// \brief Lock the cursor in this screen
		virtual void SetMouseCursorVisible(bool visible) = 0;
		virtual bool IsMouseCursorVisible() = 0;

		virtual void LockCursorInScreen(bool lock) = 0;

		virtual const glm::ivec2& GetScreenSize() const = 0;
		virtual const glm::ivec2& GetScreenCenterPosition() const = 0;
		virtual void RegisterScreenSizeListener(const ScreenSizedelegate& function) = 0;
		virtual void UnRegisterScreenSizeListener(const ScreenSizedelegate& function) = 0;

		virtual void EnableStatisticsWindow(bool enable) = 0;
		
		virtual RenderingFactory* GetFactory() = 0;
	};
}