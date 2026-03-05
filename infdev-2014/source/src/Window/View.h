#pragma once

#include "stdafx.h"
#include <vector>

#include "Rendering/Objects/RenderCamera.h"
#include "Window/IView.h"

namespace Esteem
{
	/// \brief Utility class that stores all of the views in the engine
	/// \author Y. Schaeffer
	/// \date March 2015
	class View
	{
	private:
		/// \brief Default View
		static IView* main;
		static std::vector<IView*> views;

	public:
		/// \brief Set default view
		static void SetView(IView* instance, bool noOverride = false);

		/// \brief Register view
		static void RegisterView(IView* instance);

		/// \brief Unregister view
		static void UnRegisterView(IView* instance);
		
		/// \brief Get the view
		/// \param nr	return the view on spot 'nr'
		/// \return		IView* when found and nullptr when no views are available
		static IView* GetView(int nr = 0);

		/// \brief Get amount of views
		/// \return		int amount of views
		static uint GetViewCount();

		/// \brief get state of screen focused
		static bool IsScreenFocused();
	};
}