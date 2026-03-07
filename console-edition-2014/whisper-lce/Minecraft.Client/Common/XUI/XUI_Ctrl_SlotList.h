#pragma once

// Sig: HRESULT OnGetItemCountMaxLines(XUIMessageGetItemCount *pGetItemCountData, BOOL& bHandled)
#define XUI_ON_XM_GET_ITEMCOUNT_MAX_LINES(MemberFunc)\
	if (pMessage->dwMessage == XM_GET_ITEMCOUNT && ((XUIMessageGetItemCount *) pMessage->pvData)->nType == XUI_ITEMCOUNT_MAX_LINES)\
	{\
		XUIMessageGetItemCount *pData = (XUIMessageGetItemCount *) pMessage->pvData;\
		return MemberFunc(pData, pMessage->bHandled);\
	}

// Sig: HRESULT OnGetItemCountMaxPerLine(XUIMessageGetItemCount *pGetItemCountData, BOOL& bHandled)
#define XUI_ON_XM_GET_ITEMCOUNT_MAX_PER_LINE(MemberFunc)\
	if (pMessage->dwMessage == XM_GET_ITEMCOUNT && ((XUIMessageGetItemCount *) pMessage->pvData)->nType == XUI_ITEMCOUNT_MAX_PER_LINE)\
	{\
		XUIMessageGetItemCount *pData = (XUIMessageGetItemCount *) pMessage->pvData;\
		return MemberFunc(pData, pMessage->bHandled);\
	}

class AbstractContainerMenu;
class SlotListItemControl;
class CXuiCtrlSlotItemListItem;

class CXuiCtrlSlotList : public CXuiListImpl
{
public:
	// Define the class. The class name must match the ClassOverride property
	// set for the scene in the UI Authoring tool.
	XUI_IMPLEMENT_CLASS( CXuiCtrlSlotList, L"CXuiCtrlSlotList", XUI_CLASS_LIST )

	XUI_BEGIN_MSG_MAP()
		XUI_ON_XM_INIT( OnInit )
		XUI_ON_XM_DESTROY(OnDestroy)
		XUI_ON_XM_KEYDOWN( OnKeyDown )
		XUI_ON_XM_GET_ITEMCOUNT_ALL( OnGetItemCountAll )
		XUI_ON_XM_GET_ITEMCOUNT_MAX_LINES(OnGetItemCountMaxLines)
		XUI_ON_XM_GET_ITEMCOUNT_MAX_PER_LINE(OnGetItemCountMaxPerLine)
	XUI_END_MSG_MAP()

	HRESULT OnInit( XUIMessageInit* pInitData, BOOL& bHandled );
	HRESULT OnDestroy();
	HRESULT OnKeyDown(XUIMessageInput *pInputData, BOOL& bHandled);
	HRESULT OnGetItemCountAll( XUIMessageGetItemCount *pGetItemCountData, BOOL& bHandled );
	HRESULT OnGetItemCountMaxLines(XUIMessageGetItemCount *pGetItemCountData, BOOL& bHandled);
	HRESULT OnGetItemCountMaxPerLine(XUIMessageGetItemCount *pGetItemCountData, BOOL& bHandled);
	HRESULT OnRender(XUIMessageRender *pRenderData, BOOL &rfHandled);

public:
	void SetData(int m_iPad, AbstractContainerMenu* menu, int rows, int columns, int startIndex = 0, int endIndex = 0);
	
	int GetRows() { return rows; };
	int GetColumns() { return columns; };
	int GetCurrentColumn();
	int GetCurrentRow();

	int GetCurrentIndex();

	void SetCurrentSlot(int row, int column);
	void SetEntrySlot(int row, int column, XUI_CONTROL_NAVIGATE direction);

	void Clicked();

	// 4J WESTY : Pointer Prototype : Made public.
	void GetCXuiCtrlSlotItem(int itemIndex, CXuiCtrlSlotItemListItem** CXuiCtrlSlotItem);

private:
	int slotCount;
	int rows;
	int columns;
	int startIndex;

	// 4J WESTY : Pointer Prototype : Made public.
	//void GetCXuiCtrlSlotItem(int itemIndex, CXuiCtrlSlotItemListItem** CXuiCtrlSlotItem);
};