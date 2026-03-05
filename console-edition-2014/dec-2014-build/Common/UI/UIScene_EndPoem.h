#pragma once

#include "UIScene.h"

class UIScene_EndPoem : public UIScene
{
private:
	wstring noNoiseString;
	wstring noiseString;
	vector<int> m_noiseLengths;
	bool m_bIgnoreInput;
	int m_requestedLabel;

	vector<wstring> m_paragraphs;

	IggyName m_funcSetNextLabel;
	UI_BEGIN_MAP_ELEMENTS_AND_NAMES(UIScene)
		UI_MAP_NAME(m_funcSetNextLabel, L"SetNextLabel")
	UI_END_MAP_ELEMENTS_AND_NAMES()

public:
	UIScene_EndPoem(int iPad, void *initData, UILayer *parentLayer);

	virtual EUIScene getSceneType() { return eUIScene_EndPoem;}
	virtual void updateTooltips();

protected:
	virtual wstring getMoviePath();

public:
	virtual void tick();

	// INPUT
	virtual void handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled);
	virtual void handleDestroy();

	virtual void handleRequestMoreData(F64 startIndex, bool up);

private:
	void updateNoise();
};