#pragma once

//const char* getConsoleHomePath();

DWORD GetFileSize(HANDLE fh,DWORD *pdwHigh);

DWORD XGetLanguage();
DWORD XGetLocale();
DWORD XEnableGuestSignin(BOOL fEnable);