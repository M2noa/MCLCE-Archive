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

#include "STO_DLC.h"
#include "STO_SaveGame.h"

class CStorage
{
public:
    CStorage(void);

    void Init(int (*Func)(LPVOID, const C4JStorage::ESavingMessage, int), LPVOID lpParam, LPCSTR szGroupID);
    void Tick(void);
    unsigned int CRC(unsigned char *buf, int len);
    void MakeCRCTable(void);
    unsigned int UpdateCRC(unsigned int crc, unsigned __int8 *buf, int len);
    void DebugPrintf(const char *szFormat, ...);

    BYTE gap0[8];
    CSaveGame m_SaveGame;
    CDLC m_DLC;
    BYTE gap278[0x10];
    DWORD m_CRCTable[256];
    bool m_bHasCRCTable;
};

// Singleton
extern CStorage InternalStorageManager;