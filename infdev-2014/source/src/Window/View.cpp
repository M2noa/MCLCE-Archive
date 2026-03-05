#include "View.h"

#include <algorithm>

namespace Esteem
{
	IView* View::main = nullptr;
	std::vector<IView*> View::views = std::vector<IView*>();

	void View::SetView(IView* instance, bool noOverride)
	{
		if(!(noOverride && main != nullptr))
			main = instance;

		// check if it is already present, if true remove it from the list
		auto view = std::find(views.begin(), views.end(), instance);
		if(view != views.end())
			views.erase(view);

		// insert this view on the start
		views.insert(views.begin(), instance);
	}

	void View::RegisterView(IView* instance)
	{
		// only add when it is not yet registered
		if(std::find(views.begin(), views.end(), instance) == views.end())
			views.push_back(instance);
	}

	void View::UnRegisterView(IView* instance)
	{
		auto view = std::find(views.begin(), views.end(), instance);
		if(view != views.end())
			views.erase(view);
	}

	IView* View::GetView(int nr)
	{
		return views.size() > 0 ? views[nr] : nullptr;
	}

	uint View::GetViewCount()
	{
		return views.size();
	}
	
	bool View::IsScreenFocused()
	{
		if(main == nullptr)
			return false;

		return main->IsScreenFocused();
	}
}