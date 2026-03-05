#include "stdafx.h"
#include "UI.h"
#include "UIComponent_DebugUIConsole.h"

UIComponent_DebugUIConsole::UIComponent_DebugUIConsole(int iPad, void *initData, UILayer *parentLayer) : UIScene(iPad, parentLayer)
{
	// Setup all the Iggy references we need for this scene
	initialiseMovie();

	m_bTextChanged = false;
}

wstring UIComponent_DebugUIConsole::getMoviePath()
{
	return L"DebugUIConsoleComponent";
}

void UIComponent_DebugUIConsole::tick()
{
	UIScene::tick();
	if(m_bTextChanged)
	{
		m_bTextChanged = false;
		for(unsigned int i = 0; i < 10 && i < m_textList.size(); ++i)
		{
			m_labels[i].setLabel(m_textList[i]);
		}
	}
}

void UIComponent_DebugUIConsole::addText(const string &text)
{
	if(!text.empty() && text.compare("\n") != 0)
	{
		if(m_textList.size() >= 10) m_textList.pop_front();
		m_textList.push_back(text);
		m_bTextChanged = true;
	}
}