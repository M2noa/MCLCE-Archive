#pragma once

class CXuiControl4JComboBox : public CXuiComboBoxImpl
{
public:

	// Information for one list item.
	typedef struct _LIST_ITEM_INFO 
	{
		LPCWSTR pwszText;
		LPCWSTR pwszImage;
		HXUIBRUSH hXuiBrush;
		BOOL fChecked;
		BOOL fEnabled;
	} 
	LIST_ITEM_INFO;

	// List data.
	typedef struct _tagListData 
	{
		int nItems;
		LIST_ITEM_INFO *pItems;
	} 
	LIST_DATA;	

	LIST_DATA m_ListData;
	XUI_IMPLEMENT_CLASS(CXuiControl4JComboBox, L"CXuiControl4JComboBox", XUI_CLASS_COMBOBOX);

	void SetData(_LIST_ITEM_INFO *pItems,int iCount);
	int GetSelectedIndex();

protected:

	XUI_BEGIN_MSG_MAP()
		XUI_ON_XM_INIT(OnInit)
		XUI_ON_XM_GET_SOURCE_TEXT(OnGetSourceDataText)
		XUI_ON_XM_GET_ITEMCOUNT_ALL(OnGetItemCountAll)
		XUI_ON_XM_GET_SOURCE_IMAGE(OnGetSourceDataImage)
		XUI_ON_XM_GET_ITEMENABLE(OnGetItemEnable)
		XUI_ON_XM_NOTIFY_PRESS_EX(OnNotifyPressEx)

		XUI_END_MSG_MAP()


	HRESULT OnInit( XUIMessageInit* pInitData, BOOL& bHandled );
	HRESULT OnGetSourceDataText(XUIMessageGetSourceText *pGetSourceTextData, BOOL& bHandled);
	HRESULT OnGetItemCountAll(XUIMessageGetItemCount *pGetItemCountData, BOOL& bHandled);
	HRESULT OnGetSourceDataImage(XUIMessageGetSourceImage *pGetSourceImageData,BOOL& bHandled);
	HRESULT OnGetItemEnable(XUIMessageGetItemEnable *pGetItemEnableData,BOOL& bHandled);
	HRESULT OnNotifyPressEx(HXUIOBJ hObjPressed, XUINotifyPress* pNotifyPressData,BOOL& rfHandled);


};