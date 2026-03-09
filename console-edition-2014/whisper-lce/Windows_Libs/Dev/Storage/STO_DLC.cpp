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

#include "STO_DLC.h"
#include "STO_Main.h"

XCONTENT_DATA &CDLC::GetDLC(DWORD dw)
{
    return m_vInstalledDLCs[dw];
}

CDLC::CDLC(void) : m_vInstalledDLCs(), m_szMountPath(), m_vDLCDriveMappings()
{
    m_iHasNewInstalledDLCs = false;
    dword0 = 0;
    dwordC0 = 0;
    m_iHasNewMountedDLCs = false; // @Patoke fix

    ZeroMemory(m_szDLCProductCode, sizeof(m_szDLCProductCode));
    ZeroMemory(m_szProductUpgradeKey, sizeof(m_szProductUpgradeKey));
}

C4JStorage::EDLCStatus CDLC::GetOffers(int iPad, int (*Func)(LPVOID, int, DWORD, int), LPVOID lpParam, DWORD dwOfferTypesBitmask)
{
    return C4JStorage::EDLC_NoOffers;
}

void CDLC::ClearOffers()
{
    ;
}

C4JStorage::EDLCStatus CDLC::GetInstalledDLC(int iPad, int (*Func)(LPVOID, int, int), LPVOID lpParam)
{
    if (m_iHasNewInstalledDLCs)
    {
        return C4JStorage::EDLC_Pending;
    }

    m_pInstalledDLCFunc = Func;
    m_pInstalledDLCParam = lpParam;
    m_iHasNewInstalledDLCs = true;

    bool ret = false;
    DWORD atts = GetFileAttributesA("Windows64Media/DLC");
    if (atts == -1)
    {
        atts = GetFileAttributesA("Windows64/DLC");
        ret = true;
    }

    bool validDir = atts != -1 && (atts & FILE_ATTRIBUTE_DIRECTORY);
    if (!validDir)
    {
        InternalStorageManager.DebugPrintf("No DLC directory, can't have any DLC installed\n");
        return C4JStorage::EDLC_Error;
    }

    _WIN32_FIND_DATAA hFind;
    HANDLE hFindFile;
    if (ret)
    {
        hFindFile = FindFirstFileA("Windows64/DLC/*", &hFind);
    }
    else
    {
        hFindFile = FindFirstFileA("Windows64Media/DLC/*", &hFind);
    }

    if (hFindFile != (HANDLE)-1LL)
    {
        do
        {
            atts = hFind.dwFileAttributes;

            bool isArt = hFind.dwFileAttributes != -1 && (hFind.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
            if (isArt && hFind.cFileName[0] != '.')
            {
                XCONTENT_DATA data;

                if (ret)
                {
                    sprintf(data.szFileName, "Windows64/DLC/%s", hFind.cFileName);
                }
                else
                {
                    sprintf(data.szFileName, "Windows64Media/DLC/%s", hFind.cFileName);
                }

                swprintf(data.szDisplayName, 256, L"%S", hFind.cFileName);
                int displayNameLen = wcslen(data.szDisplayName);

                data.DeviceID = 0;
                data.dwContentType = 0;

                AddInstalled(&data);
            }
        } while (FindNextFileA(hFindFile, &hFind));
        FindClose(hFindFile);
    }

    return C4JStorage::EDLC_Idle;
}

DWORD CDLC::MountInstalledDLC(int iPad, DWORD dwDLC, int (*Func)(LPVOID, int, DWORD, DWORD), LPVOID lpParam, LPCSTR szMountDrive)
{
    this->m_pMountedDLCFunc = Func;
    this->m_pMountedDLCParam = lpParam;

    if (szMountDrive)
    {
        m_szMountPath = szMountDrive;
    }
    else
    {
        m_szMountPath = this->m_szPackageRoot;
    }

    this->m_uiCurrentMappedDLC = dwDLC;

    char *dlcdirPath = m_vInstalledDLCs[m_uiCurrentMappedDLC].szFileName;
    m_vDLCDriveMappings.push_back(DriveMapping(dlcdirPath, m_szMountPath));

    m_iHasNewMountedDLCs = true;

    return 997;
}

DWORD CDLC::UnmountInstalledDLC(LPCSTR szMountDrive)
{
    LPCSTR szDrive = nullptr;

    if (szMountDrive)
    {
        szDrive = szMountDrive;
    }
    else
    {
        szDrive = this->m_szPackageRoot;
    }

    for (int i = 0; i < this->m_vDLCDriveMappings.size(); i++)
    {
        if (m_vDLCDriveMappings[i].m_szDirectoryPath == szDrive)
        {
            m_vDLCDriveMappings.erase(m_vDLCDriveMappings.begin() + i);

            return 0;
        }
    }
    return 0;
}

void CDLC::GetMountedDLCFileList(const char *szMountDrive, std::vector<std::string> &fileList)
{
    char *dlcdirPath = new char[256];
    sprintf(dlcdirPath, "%s/*", m_vInstalledDLCs[m_uiCurrentMappedDLC].szFileName);

    _WIN32_FIND_DATAA atts;
    HANDLE hFind = FindFirstFileA(dlcdirPath, &atts);
    if (hFind != (HANDLE)-1LL)
    {
        do
        {
            if (atts.dwFileAttributes == -1 || (atts.dwFileAttributes & 0x10) != 0x10)
            {
                char dir[256];
                sprintf(dir, "%s/%s", m_vInstalledDLCs[m_uiCurrentMappedDLC].szFileName, atts.cFileName);

                fileList.push_back(dir);
            }
        } while (FindNextFileA(hFind, &atts));
        FindClose(hFind);
    }
    delete[] dlcdirPath;
}

std::string CDLC::GetMountedPath(std::string szMount)
{
    for (int ch = 0; ch < szMount.size(); ++ch)
    {
        if (szMount[ch] == '/' || szMount[ch] == '\\')
        {
            return "";
        }

        if (szMount[ch] == ':')
        {
            std::string driveName = szMount.substr(0, ch);
            for (int i = 0; i < m_vDLCDriveMappings.size(); i++)
            {
                if (m_vDLCDriveMappings[i].m_szMountPath == driveName)
                {
                    std::string newPath = m_vDLCDriveMappings[i].m_szDirectoryPath;

                    newPath.append(szMount.substr(ch + 1, -1));

                    return newPath;
                }
            }
            break;
        }
    }

    return "";
}

void CDLC::SetDLCProductCode(const char *szProductCode)
{
    strcpy(m_szDLCProductCode, szProductCode);
}

void CDLC::SetProductUpgradeKey(const char *szProductCode)
{
    strcpy(m_szProductUpgradeKey, szProductCode);
}

int CDLC::GetAvailableDLCCount(int iPad)
{
    return 0;
}

void CDLC::SetPackageRoot(char *pszDLCRoot)
{
    strcpy(this->m_szPackageRoot, pszDLCRoot);
}

void CDLC::Tick(void)
{
    if (m_iHasNewInstalledDLCs)
    {
        m_iHasNewInstalledDLCs = false;
        m_pInstalledDLCFunc(m_pInstalledDLCParam, m_vInstalledDLCs.size(), 0);
    }
    if (m_iHasNewMountedDLCs)
    {
        m_iHasNewMountedDLCs = false;
        m_pMountedDLCFunc(m_pMountedDLCParam, 0, 0, m_dwLicenseMask);
    }
}

void CDLC::AddInstalled(XCONTENT_DATA *data)
{
    m_vInstalledDLCs.push_back(*data);
}

DWORD CDLC::CancelOffers(void)
{
    return 0;
}
