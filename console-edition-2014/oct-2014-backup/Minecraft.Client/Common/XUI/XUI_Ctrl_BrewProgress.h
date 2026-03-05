#pragma once
using namespace std;

#include "XUI_Ctrl_ProgressCtrlBase.h"

class CXuiCtrlBrewProgress : public CXuiCtrlProgressCtrlBase
{
public:
	// Define the class. The class name must match the ClassOverride property
	// set for the scene in the UI Authoring tool.
	XUI_IMPLEMENT_CLASS( CXuiCtrlBrewProgress, L"CXuiCtrlBrewProgress", XUI_CLASS_PROGRESSBAR  )

	XUI_BEGIN_MSG_MAP()
		XUI_ON_XM_GET_SOURCE_TEXT(OnGetSourceDataText)
	XUI_END_MSG_MAP()

	virtual int GetValue();
	virtual void GetRange(int *pnRangeMin, int *pnRangeMax);
};