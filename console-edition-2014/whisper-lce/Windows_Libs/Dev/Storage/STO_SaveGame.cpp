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

#include "STO_SaveGame.h"
#include "STO_Main.h"

// @Patoke add
char CSaveGame::szPNGHeader[] = "\x89\x50\x4E\x47\x0D\x0A\x1A\x0A";

CSaveGame::CSaveGame()
{
    m_pSaveData = nullptr;
    m_uiSaveSize = 0;
    m_bIsSafeDisabled = false;

    ZeroMemory(m_szSaveUniqueName, sizeof(m_szSaveUniqueName));
    ZeroMemory(m_szSaveTitle, sizeof(m_szSaveTitle));

    m_pSaveDetails = nullptr;
    m_bHasSaveDetails = false;

    GetCurrentDirectoryA(sizeof(m_szSaveUniqueName), m_szSaveUniqueName);

    char dirName[256];
    char curDir[256];
    GetCurrentDirectoryA(sizeof(dirName), dirName);
    sprintf(curDir, "%s/Windows64/GameHDD/", dirName);
    CreateDirectoryA(curDir, 0);

    // @Patoke add
    this->m_pbThumbnailData = nullptr;
    this->m_uiThumbnailSize = 0;
    this->m_pbImageData = nullptr;
    this->m_uiImageSize = 0;
}

void CSaveGame::SetSaveDisabled(bool bDisable)
{
    m_bIsSafeDisabled = bDisable;
}

bool CSaveGame::GetSaveDisabled(void)
{
    return m_bIsSafeDisabled;
}

void CSaveGame::ResetSaveData()
{
    free(m_pSaveData);
    m_pSaveData = nullptr;
    m_uiSaveSize = 0;
}

C4JStorage::ESaveGameState CSaveGame::GetSavesInfo(int iPad, int (*Func)(LPVOID lpParam, SAVE_DETAILS *pSaveDetails, const bool), LPVOID lpParam,
                                                   char *pszSavePackName)
{
    WIN32_FIND_DATAA findFileData;
    WIN32_FILE_ATTRIBUTE_DATA fileInfoBuffer;

    if (!m_pSaveDetails)
    {
        m_pSaveDetails = new SAVE_DETAILS();
        memset(m_pSaveDetails, 0, sizeof(SAVE_DETAILS));
    }

    delete[] m_pSaveDetails->SaveInfoA;
    m_pSaveDetails->SaveInfoA = nullptr;
    m_pSaveDetails->iSaveC = 0;

    char dirName[256];
    char curDir[256];
    GetCurrentDirectoryA(sizeof(dirName), dirName);
    sprintf(curDir, "%s\\Windows64\\GameHDD\\*", dirName);

    int resultCount = 0;
    HANDLE h = FindFirstFileExA(curDir, FindExInfoStandard, &findFileData, FindExSearchLimitToDirectories, 0, 0);
    if (h == INVALID_HANDLE_VALUE)
    {
        DWORD error = GetLastError();
        printf("Error finding save dirs: 0x%08x\n", error);
    }
    else
    {
        do
        {
            if ((findFileData.dwFileAttributes & 0x10) != 0)
            {
                resultCount++;
            }
        } while (FindNextFileA(h, &findFileData));
        FindClose(h);
    }

    if (resultCount > 0)
    {
        m_pSaveDetails->SaveInfoA = new SAVE_INFO[resultCount];
        memset(m_pSaveDetails->SaveInfoA, 0, sizeof(SAVE_INFO) * resultCount);

        m_pSaveDetails->iSaveC = 0;
        int i = 0;
        HANDLE fi = FindFirstFileExA(curDir, FindExInfoStandard, &findFileData, FindExSearchLimitToDirectories, 0, 0);
        if (fi != INVALID_HANDLE_VALUE)
        {
            do
            {
                if ((findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 && strcmp(findFileData.cFileName, ".") &&
                    strcmp(findFileData.cFileName, ".."))
                {
                    strcpy_s(m_pSaveDetails->SaveInfoA[i].UTF8SaveFilename, findFileData.cFileName);

                    // @Patoke add: we want to preserve the title name, so we save this in the actual save file name
                    char searchPath[280];
                    sprintf(searchPath, "%s\\Windows64\\GameHDD\\%s\\*", dirName, findFileData.cFileName);

                    WIN32_FIND_DATAA saveFileData;
                    HANDLE hSaveFile = FindFirstFileA(searchPath, &saveFileData);

                    char szTitleName[256] = {0};

                    if (hSaveFile != INVALID_HANDLE_VALUE)
                    {
                        do
                        {
                            // @Patoke todo: we assume the first actual file here to be the save file, ideally we would want to check the extension
                            // too but this is good enough for now
                            if (!(saveFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                            {
                                // remove the file extension
                                std::string decoratedSaveTitle = saveFileData.cFileName;
                                uint64_t extensionDot = decoratedSaveTitle.rfind('.');

                                // populate the save title
                                if (extensionDot != std::string::npos)
                                {
                                    strcpy_s(this->m_szSaveTitle, decoratedSaveTitle.substr(0, extensionDot).c_str());
                                }
                                else
                                {
                                    strcpy_s(this->m_szSaveTitle, decoratedSaveTitle.c_str());
                                }

                                strcpy_s(szTitleName, sizeof(szTitleName), this->m_szSaveTitle);
                                break;
                            }
                        } while (FindNextFileA(hSaveFile, &saveFileData));

                        FindClose(hSaveFile);
                    }

                    // set the actual save file name as the title now, before we would use the folder name which is the unique save name
                    strcpy_s(m_pSaveDetails->SaveInfoA[i].UTF8SaveTitle, szTitleName);

                    char fileName[280];
                    sprintf(fileName, "%s\\Windows64\\GameHDD\\%s\\%s.ms", dirName, findFileData.cFileName, szTitleName);

                    GetFileAttributesExA(fileName, GetFileExInfoStandard, &fileInfoBuffer);
                    m_pSaveDetails->SaveInfoA[i].metaData.dataSize = fileInfoBuffer.nFileSizeLow;

                    // @Patoke todo: a save can have multiple thumbnails, implement this behaviour
                    char thumbName[280];
                    sprintf(thumbName, "%s\\Windows64\\GameHDD\\%s\\thumbnails\\thumbData.png", dirName, findFileData.cFileName);

                    GetFileAttributesExA(thumbName, GetFileExInfoStandard, &fileInfoBuffer);
                    m_pSaveDetails->SaveInfoA[i++].metaData.thumbnailSize = fileInfoBuffer.nFileSizeLow;

                    m_pSaveDetails->iSaveC++;
                }
            } while (FindNextFileA(fi, &findFileData));
            FindClose(fi);
        }
    }

    m_bHasSaveDetails = true;
    if (Func)
    {
        Func(lpParam, m_pSaveDetails, true);
    }

    return C4JStorage::ESaveGame_Idle;
}

PSAVE_DETAILS CSaveGame::ReturnSavesInfo()
{
    if (m_bHasSaveDetails)
        return m_pSaveDetails;
    else
        return nullptr;
}

void CSaveGame::ClearSavesInfo()
{
    m_bHasSaveDetails = false;
    if (m_pSaveDetails)
    {
        if (m_pSaveDetails->SaveInfoA)
        {
            delete[] m_pSaveDetails->SaveInfoA;
            m_pSaveDetails->SaveInfoA = nullptr;
            m_pSaveDetails->iSaveC = 0;
        }
        delete m_pSaveDetails;
        m_pSaveDetails = 0;
    }
}

// @Patoke add
C4JStorage::ESaveGameState CSaveGame::LoadSaveDataThumbnail(PSAVE_INFO pSaveInfo,
                                                            int (*Func)(LPVOID lpParam, PBYTE pbThumbnail, DWORD dwThumbnailBytes), LPVOID lpParam)
{
    if (pSaveInfo == nullptr)
    {
        return C4JStorage::ESaveGame_Idle;
    }

    DWORD thumbSize = pSaveInfo->metaData.thumbnailSize;
    if (thumbSize > 0 && pSaveInfo->thumbnailData == nullptr)
    {
        char curDir[256];
        GetCurrentDirectoryA(sizeof(curDir), curDir);

        const char *saveName = (const char *)pSaveInfo;

        char thumbPath[512];
        sprintf(thumbPath, "%s/Windows64/GameHDD/%s/thumbnails/thumbData.png", curDir, saveName);

        HANDLE hThumb = CreateFileA(thumbPath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

        if (hThumb != INVALID_HANDLE_VALUE)
        {
            pSaveInfo->thumbnailData = new BYTE[thumbSize];

            DWORD bytesRead = 0;
            BOOL res = ReadFile(hThumb, pSaveInfo->thumbnailData, thumbSize, &bytesRead, 0);

            // If the read fails or is incomplete, clean up to prevent corrupted image data
            if (!res || bytesRead != thumbSize)
            {
                delete[] pSaveInfo->thumbnailData;
                pSaveInfo->thumbnailData = nullptr;
            }

            CloseHandle(hThumb);
        }
    }

    Func(lpParam, pSaveInfo->thumbnailData, pSaveInfo->metaData.thumbnailSize);
    return C4JStorage::ESaveGame_GetSaveThumbnail;
}

C4JStorage::ESaveGameState CSaveGame::LoadSaveData(PSAVE_INFO pSaveInfo, int (*Func)(LPVOID lpParam, const bool, const bool), LPVOID lpParam)
{
    SetSaveUniqueFilename(pSaveInfo->UTF8SaveFilename);

    memcpy(this->m_szSaveTitle, pSaveInfo->UTF8SaveTitle, sizeof(this->m_szSaveTitle)); // @Patoke add

    if (m_pSaveData)
    {
        free(m_pSaveData);
    }

    m_pSaveData = malloc(pSaveInfo->metaData.dataSize);
    m_uiSaveSize = pSaveInfo->metaData.dataSize;

    char dirName[256];
    char curDir[256];
    char fileName[280];
    GetCurrentDirectoryA(sizeof(curDir), curDir);
    sprintf(dirName, "%s/Windows64/GameHDD/%s", curDir, m_szSaveUniqueName);
    CreateDirectoryA(dirName, 0);
    sprintf(fileName, "%s/%s.ms", dirName, this->m_szSaveTitle); // @Patoke add

    HANDLE h = CreateFileA(fileName, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    bool success = false;
    if (h != INVALID_HANDLE_VALUE)
    {
        DWORD bytesRead = 0;
        BOOL res = ReadFile(h, m_pSaveData, m_uiSaveSize, &bytesRead, 0);
        _ASSERT(res && bytesRead == m_uiSaveSize);
        CloseHandle(h);
        success = true;
    }

    if (Func)
    {
        Func(lpParam, 0, success);
    }

    return C4JStorage::ESaveGame_Idle;
}

unsigned int CSaveGame::GetSaveSize()
{
    return m_uiSaveSize;
}

void CSaveGame::GetSaveData(void *pvData, unsigned int *puiBytes)
{
    if (pvData)
    {
        memmove(pvData, m_pSaveData, m_uiSaveSize);
        *puiBytes = m_uiSaveSize;
    }
    else
    {
        *puiBytes = 0;
    }
}

// @Patoke add
bool CSaveGame::GetSaveUniqueNumber(INT *piVal)
{
    if (m_szSaveUniqueName[0] == '\0')
    {
        return 0;
    }

    int year, month, day, hour, minute;
    sscanf(&m_szSaveUniqueName[4], "%02d%02d%02d%02d%02d", &year, &month, &day, &hour, &minute);

    *piVal = 2678400 * year + 86400 * month + 3600 * day + 60 * hour + minute;

    return true;
}

// @Patoke add
bool CSaveGame::GetSaveUniqueFilename(char *pszName)
{
    if (m_szSaveUniqueName[0] == '\0')
    {
        return false;
    }

    memset(pszName, 0, 14);
    for (int i = 0; i < 12; i++)
    {
        pszName[i] = m_szSaveUniqueName[i + 2];
    }

    return true;
}

void CSaveGame::SetSaveTitle(LPCWSTR pwchDefaultSaveName)
{
    CreateSaveUniqueName();
    sprintf(m_szSaveTitle, "%S", pwchDefaultSaveName); // @Patoke add
}

PVOID CSaveGame::AllocateSaveData(unsigned int uiBytes)
{
    free(m_pSaveData);

    m_pSaveData = malloc(uiBytes);
    if (m_pSaveData)
    {
        m_uiSaveSize = uiBytes;
    }

    return m_pSaveData;
}

// @Patoke add
void CSaveGame::SetSaveImages(PBYTE pbThumbnail, DWORD dwThumbnailBytes, PBYTE pbImage, DWORD dwImageBytes, PBYTE pbTextData, DWORD dwTextDataBytes)
{
    if (this->m_pbThumbnailData)
    {
        free(this->m_pbThumbnailData);
    }

    this->m_pbImageData = pbImage;
    this->m_uiImageSize = dwImageBytes;

    DWORD dwNewThumbnailBytes = dwThumbnailBytes;
    if (dwTextDataBytes > 0)
    {
        // add extra bytes to the allocation for the text chunk (4 bytes for size, 4 bytes for type, 4 bytes for checksum)
        dwNewThumbnailBytes += dwTextDataBytes + 12;
    }

    // allocate the thumbnail
    this->m_pbThumbnailData = static_cast<PBYTE>(malloc(dwNewThumbnailBytes));
    this->m_uiThumbnailSize = dwNewThumbnailBytes;
    memset(this->m_pbThumbnailData, 0, dwNewThumbnailBytes);

    // copy original thumbnail data to new buffer
    memcpy(this->m_pbThumbnailData, pbThumbnail, dwThumbnailBytes);

    // inject text metadata into the thumbnail if it exists
    if (dwTextDataBytes > 0)
    {
        this->AddTextFieldToPNG(this->m_pbThumbnailData, dwThumbnailBytes, pbTextData, dwTextDataBytes, dwNewThumbnailBytes);
    }
}

// @Patoke add
void CSaveGame::AddTextFieldToPNG(PBYTE pbImageData, DWORD dwImageBytes, PBYTE pbTextData, DWORD dwTextBytes, DWORD dwTotalSizeAllocated)
{
    if (dwImageBytes == 0)
    {
        return;
    }

    for (int j = 0; j < 8; ++j)
    {
        if (CSaveGame::szPNGHeader[j] != pbImageData[j])
        {
            return;
        }
    }

    unsigned int offset = 8;
    while (offset < dwImageBytes)
    {
        unsigned int chunkStart = offset;

        unsigned int chunkLength = this->ReverseBytes(*reinterpret_cast<unsigned int *>(&pbImageData[offset]));
        offset += 4;

        unsigned int chunkType = this->ReverseBytes(*reinterpret_cast<unsigned int *>(&pbImageData[offset]));
        offset += 4;

        if (chunkType == 'IEND')
        {
            offset = chunkStart;

            // write the tEXt chunk before the IEND chunk
            *reinterpret_cast<unsigned int *>(&pbImageData[offset]) = this->ReverseBytes(static_cast<unsigned int>(dwTextBytes));
            offset += 4;

            unsigned __int8 *textTypeStart = &pbImageData[offset];
            *reinterpret_cast<unsigned int *>(&pbImageData[offset]) = this->ReverseBytes('tEXt');
            offset += 4;

            memcpy(&pbImageData[offset], pbTextData, dwTextBytes);
            offset += dwTextBytes;

            unsigned int textCrc = InternalStorageManager.CRC(textTypeStart, dwTextBytes + 4);
            *reinterpret_cast<unsigned int *>(&pbImageData[offset]) = this->ReverseBytes(textCrc);
            offset += 4;

            // add a new IEND chunk
            *reinterpret_cast<unsigned int *>(&pbImageData[offset]) = 0;
            offset += 4;

            unsigned __int8 *iendTypeStart = &pbImageData[offset];
            *reinterpret_cast<unsigned int *>(&pbImageData[offset]) = this->ReverseBytes('IEND');
            offset += 4;

            unsigned int iendCrc = InternalStorageManager.CRC(iendTypeStart, 4);
            *reinterpret_cast<unsigned int *>(&pbImageData[offset]) = this->ReverseBytes(iendCrc);
            offset += 4;

            assert("uiCount <= dwTotalSizeAllocated");

            break;
        }
        else
        {
            // not 'IEND' chunk, continue to next chunk
            offset += chunkLength + 4;
        }
    }
}

// @Patoke add
unsigned int CSaveGame::ReverseBytes(unsigned int uiValue)
{
    unsigned int uiReturn = 0;

    uiReturn = (uiValue << 24) | ((uiValue << 8) & 0x00FF0000) | ((uiValue >> 8) & 0x0000FF00) | (uiValue >> 24);

    return uiReturn;
}

C4JStorage::ESaveGameState CSaveGame::SaveSaveData(int (*Func)(LPVOID, const bool), LPVOID lpParam)
{
    char dirName[256];
    char curDir[256];
    char fileName[280];
    char thumbName[280];

    GetCurrentDirectoryA(sizeof(curDir), curDir);
    sprintf(dirName, "%s/Windows64/GameHDD/%s", curDir, m_szSaveUniqueName);
    CreateDirectoryA(dirName, 0);
    sprintf(fileName, "%s/%s.ms", dirName, this->m_szSaveTitle); // @Patoke add

    HANDLE h = CreateFileA(fileName, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    DWORD bytesWritten = 0;
    BOOL res = WriteFile(h, m_pSaveData, m_uiSaveSize, &bytesWritten, 0);
    _ASSERT(res && bytesWritten == m_uiSaveSize);

    CloseHandle(h);

    // @Patoke add
    if (this->m_pbThumbnailData != nullptr && this->m_uiThumbnailSize > 0)
    {
        char thumbDir[280];
        sprintf(thumbDir, "%s/thumbnails", dirName);
        CreateDirectoryA(thumbDir, 0);

        sprintf(thumbName, "%s/thumbnails/thumbData.png", dirName);

        HANDLE hThumb = CreateFileA(thumbName, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

        DWORD thumbBytesWritten = 0;
        BOOL thumbRes = WriteFile(hThumb, this->m_pbThumbnailData, this->m_uiThumbnailSize, &thumbBytesWritten, 0);
        _ASSERT(thumbRes && thumbBytesWritten == this->m_uiThumbnailSize);

        CloseHandle(hThumb);
    }

    Func(lpParam, true);

    return C4JStorage::ESaveGame_Idle;
}

// @Patoke add
C4JStorage::ESaveGameState CSaveGame::DeleteSaveData(PSAVE_INFO pSaveInfo, int (*Func)(LPVOID lpParam, const bool), LPVOID lpParam)
{
    char dirName[256];
    char curDir[256];
    char fileName[280];
    char thumbName[280];

    GetCurrentDirectoryA(sizeof(curDir), curDir);

    sprintf(dirName, "%s/Windows64/GameHDD/%s", curDir, pSaveInfo->UTF8SaveFilename);
    sprintf(fileName, "%s/%s.ms", dirName, pSaveInfo->UTF8SaveTitle);
    sprintf(thumbName, "%s/thumbnails/thumbData.png", dirName);

    DeleteFileA(fileName);
    DeleteFileA(thumbName);
    RemoveDirectoryA(dirName);

    PSAVE_INFO m_pDeleteInfo = pSaveInfo; // only here for consistency with the xbox one assert
    assert((m_pDeleteInfo >= &m_pSaveDetails->SaveInfoA[0]) && (m_pDeleteInfo < &m_pSaveDetails->SaveInfoA[m_pSaveDetails->iSaveC]));

    uint64_t index = pSaveInfo - this->m_pSaveDetails->SaveInfoA;

    // shift all save data by 1 to fill the gap
    for (int j = index; j < this->m_pSaveDetails->iSaveC - 1; ++j)
    {
        this->m_pSaveDetails->SaveInfoA[j] = this->m_pSaveDetails->SaveInfoA[j + 1];
    }

    --this->m_pSaveDetails->iSaveC;

    // not calling this function is what caused the softlock in the original binaries
    if (Func)
    {
        Func(lpParam, true);
    }

    return C4JStorage::ESaveGame_Idle;
}

void CSaveGame::SetSaveUniqueFilename(char *szFilename)
{
    strcpy_s(m_szSaveUniqueName, szFilename);
}

void CSaveGame::CreateSaveUniqueName(void)
{
    _SYSTEMTIME UTCSysTime;
    GetSystemTime(&UTCSysTime);

    sprintf_s(m_szSaveUniqueName, sizeof(m_szSaveUniqueName), "%4d%02d%02d%02d%02d%02d", UTCSysTime.wYear, UTCSysTime.wMonth, UTCSysTime.wDay,
              UTCSysTime.wHour, UTCSysTime.wMinute, UTCSysTime.wSecond);
}
