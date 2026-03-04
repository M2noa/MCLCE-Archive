#pragma once

class CXuiCtrlProgressCtrlBase : public CXuiProgressBar, public CXuiElementImplBase
{
public:
	HRESULT OnGetSourceDataText(XUIMessageGetSourceText *pGetSourceTextData, BOOL& bHandled);

	// Override these in the derived classes to return the values to be displayed on the control
	virtual int GetValue() = 0;
	virtual void GetRange(int *pnRangeMin, int *pnRangeMax) = 0;
};