#pragma once

class IUIScene_PauseMenu
{
protected:
	DLCPack *m_pDLCPack;

public:
	static int ExitGameDialogReturned(void *pParam,int iPad,C4JStorage::EMessageResult result);
	static int ExitGameSaveDialogReturned(void *pParam,int iPad,C4JStorage::EMessageResult result);
	static int ExitGameAndSaveReturned(void *pParam,int iPad,C4JStorage::EMessageResult result);
	static int ExitGameDeclineSaveReturned(void *pParam,int iPad,C4JStorage::EMessageResult result);
	static int WarningTrialTexturePackReturned(void *pParam,int iPad,C4JStorage::EMessageResult result);
	static int SaveGameDialogReturned(void *pParam,int iPad,C4JStorage::EMessageResult result);
	static int EnableAutosaveDialogReturned(void *pParam,int iPad,C4JStorage::EMessageResult result);
	static int DisableAutosaveDialogReturned(void *pParam,int iPad,C4JStorage::EMessageResult result);

	static int SaveWorldThreadProc( void* lpParameter );
	static int ExitWorldThreadProc( void* lpParameter );
	static void _ExitWorld(LPVOID lpParameter); // Call only from a thread

protected:
	virtual void ShowScene(bool show) = 0;
	virtual void SetIgnoreInput(bool ignoreInput) = 0;
};