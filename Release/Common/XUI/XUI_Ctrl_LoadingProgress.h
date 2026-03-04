#pragma once

#include "XUI_Ctrl_ProgressCtrlBase.h"

class CXuiCtrlLoadingProgress : public CXuiCtrlProgressCtrlBase
{
public:
	// Define the class. The class name must match the ClassOverride property
	// set for the scene in the UI Authoring tool.
	XUI_IMPLEMENT_CLASS( CXuiCtrlLoadingProgress, L"CXuiCtrlLoadingProgress", XUI_CLASS_PROGRESSBAR  )

	XUI_BEGIN_MSG_MAP()
		XUI_ON_XM_GET_SOURCE_TEXT(OnGetSourceDataText)
	XUI_END_MSG_MAP()

	virtual int GetValue();
	virtual void GetRange(int *pnRangeMin, int *pnRangeMax);
};