#pragma once
/*
MIT License

Copyright (c) 2026 Patoke

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "4J_Storage.h"

class CSaveGame
{
public:
    CSaveGame();
    void SetSaveDisabled(bool bDisable);
    bool GetSaveDisabled(void);

    void ResetSaveData();
    C4JStorage::ESaveGameState GetSavesInfo(int iPad, int (*Func)(LPVOID lpParam, SAVE_DETAILS *pSaveDetails, const bool), LPVOID lpParam,
                                            char *pszSavePackName);
    PSAVE_DETAILS ReturnSavesInfo();
    void ClearSavesInfo();
    C4JStorage::ESaveGameState LoadSaveDataThumbnail(PSAVE_INFO pSaveInfo, int (*Func)(LPVOID lpParam, PBYTE pbThumbnail, DWORD dwThumbnailBytes),
                                                     LPVOID lpParam);
    C4JStorage::ESaveGameState LoadSaveData(PSAVE_INFO pSaveInfo, int (*Func)(LPVOID lpParam, const bool, const bool), LPVOID lpParam);
    unsigned int GetSaveSize();
    void GetSaveData(void *pvData, unsigned int *puiBytes);
    bool GetSaveUniqueNumber(INT *piVal);
    bool GetSaveUniqueFilename(char *pszName);
    void SetSaveTitle(LPCWSTR pwchDefaultSaveName);
    PVOID AllocateSaveData(unsigned int uiBytes);
    void SetSaveImages(PBYTE pbThumbnail, DWORD dwThumbnailBytes, PBYTE pbImage, DWORD dwImageBytes, PBYTE pbTextData, DWORD dwTextDataBytes);
    C4JStorage::ESaveGameState SaveSaveData(int (*Func)(LPVOID, const bool), LPVOID lpParam);
    C4JStorage::ESaveGameState DeleteSaveData(PSAVE_INFO pSaveInfo, int (*Func)(LPVOID lpParam, const bool), LPVOID lpParam);
    void SetSaveUniqueFilename(char *szFilename);

    void CreateSaveUniqueName(void);

    // @Patoke add: definition taken from the Xbox One binaries
    void AddTextFieldToPNG(PBYTE pbImageData, DWORD dwImageBytes, PBYTE pbTextData, DWORD dwTextBytes, DWORD dwTotalSizeAllocated);
    unsigned int ReverseBytes(unsigned int uiValue);

    void *m_pSaveData;
    unsigned int m_uiSaveSize;
    char m_szSaveUniqueName[32];
    char m_szSaveTitle[256];
    bool m_bIsSafeDisabled;
    bool m_bHasSaveDetails;
    SAVE_DETAILS *m_pSaveDetails;

    // @Patoke add
    PBYTE m_pbThumbnailData;
    unsigned int m_uiThumbnailSize;

    PBYTE m_pbImageData;
    unsigned int m_uiImageSize;

    static char szPNGHeader[];
};