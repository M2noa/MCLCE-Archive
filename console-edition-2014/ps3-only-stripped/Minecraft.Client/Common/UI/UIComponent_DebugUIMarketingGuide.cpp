#include "stdafx.h"
#include "UI.h"
#include "UIComponent_DebugUIMarketingGuide.h"

UIComponent_DebugUIMarketingGuide::UIComponent_DebugUIMarketingGuide(int iPad, void *initData, UILayer *parentLayer) : UIScene(iPad, parentLayer)
{
	// Setup all the Iggy references we need for this scene
	initialiseMovie();

	IggyDataValue result;
	IggyDataValue value[1];
	value[0].type = IGGY_DATATYPE_number;
	value[0].number = (F64)0;	// WIN64
#if defined __PS3__
	value[0].number = (F64)3;
#endif
	IggyResult out = IggyPlayerCallMethodRS ( getMovie() , &result, IggyPlayerRootPath( getMovie() ), m_funcSetPlatform , 1 , value );
}

wstring UIComponent_DebugUIMarketingGuide::getMoviePath()
{
	return L"DebugUIMarketingGuide";
}