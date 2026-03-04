#pragma once


class CXuiCtrl4JList : public CXuiListImpl
{
public:
	enum
	{
		eSortList_Date = 0,
		eSortList_Alphabetical,
		eSortList_Index,
	};

	// Information for one list item.
	typedef struct _LIST_ITEM_INFO 
	{
		LPCWSTR pwszText;
		LPCWSTR pwszImage;
		HXUIBRUSH hXuiBrush;
		BOOL fChecked;
		BOOL fEnabled;
		bool bIsDamaged; // damaged save
		FILETIME fTime;
		int iData; // user data
		int iIndex; // used for internal list sorting
		int iSortIndex; // used to force an order for DLC
	} 
	LIST_ITEM_INFO,*PLIST_ITEM_INFO;

	typedef std::vector <PLIST_ITEM_INFO> LISTITEMINFOARRAY;

	XUI_IMPLEMENT_CLASS(CXuiCtrl4JList, L"CXuiCtrl4JList", XUI_CLASS_LIST);

	void AddData( const LIST_ITEM_INFO& ItemInfo , int iSortListFromIndex=-1, int iSortFunction=CXuiCtrl4JList::eSortList_Date);
	void RemoveAllData( );
	void UpdateText(int iItem,LPCWSTR pwszText );
	void SelectByUserData(int iData);
	int GetIndexByUserData(int iData);

	void UpdateGraphic(int iItem,HXUIBRUSH hXuiBrush );
	void UpdateGraphic(FILETIME *pfTime,HXUIBRUSH hXuiBrush );
	void UpdateGraphicFromiData(int iData,HXUIBRUSH hXuiBrush );
	LIST_ITEM_INFO&	GetData(DWORD dw);
	LIST_ITEM_INFO&	GetData(FILETIME *pFileTime); 
	LIST_ITEM_INFO&	GetDataiData(int iData); 
	HRESULT SetBorder(DWORD dw,BOOL bShow); // for a highlight around the current selected item in the controls layout
	void SetSelectionChangedHandle(HXUIOBJ hObj);
protected:

	XUI_BEGIN_MSG_MAP()
		XUI_ON_XM_INIT(OnInit)
		XUI_ON_XM_GET_SOURCE_TEXT(OnGetSourceDataText)
		XUI_ON_XM_GET_ITEMCOUNT_ALL(OnGetItemCountAll)
		XUI_ON_XM_GET_SOURCE_IMAGE(OnGetSourceDataImage)
		XUI_ON_XM_GET_ITEMENABLE(OnGetItemEnable)
		XUI_ON_XM_DESTROY( OnDestroy )
		XUI_ON_XM_NOTIFY_SELCHANGED( OnNotifySelChanged )
	XUI_END_MSG_MAP()


	HRESULT OnInit( XUIMessageInit* pInitData, BOOL& bHandled );
	HRESULT OnGetSourceDataText(XUIMessageGetSourceText *pGetSourceTextData, BOOL& bHandled);
	HRESULT OnGetItemCountAll(XUIMessageGetItemCount *pGetItemCountData, BOOL& bHandled);
	HRESULT OnGetSourceDataImage(XUIMessageGetSourceImage *pGetSourceImageData,BOOL& bHandled);
	HRESULT OnGetItemEnable(XUIMessageGetItemEnable *pGetItemEnableData,BOOL& bHandled);
	HRESULT OnDestroy();
	HRESULT OnNotifySelChanged( HXUIOBJ hObjSource, XUINotifySelChanged* pNotifySelChangedData, BOOL& bHandled );

	LISTITEMINFOARRAY m_vListData;
	CRITICAL_SECTION m_AccessListData;

private:
	static bool AlphabeticSortFn(const void *a, const void *b);
	static bool TimeSortFn(const void *a, const void *b);
	static bool IndexSortFn(const void *a, const void *b);

	HXUIOBJ m_hSelectionChangedHandlerObj;
};
