#include "stdafx.h"

#include "XUI_Ctrl_ProgressCtrlBase.h"

HRESULT CXuiCtrlProgressCtrlBase::OnGetSourceDataText(XUIMessageGetSourceText *pGetSourceTextData, BOOL& bHandled)
{
	// The Xui backend calls GetSourceDataText every frame to get the text for the indexed label
	// We don't want to change the label, but take this opportunity to send out a message to ourself
	// to update the value of the progress bar
	this->SetValue( GetValue() );

	int min, max;
	this->GetRange( &min, &max );
	this->SetRange( min, max );

	pGetSourceTextData->szText = L"";
	pGetSourceTextData->bDisplay = FALSE;

	bHandled = TRUE;

	return S_OK;
}