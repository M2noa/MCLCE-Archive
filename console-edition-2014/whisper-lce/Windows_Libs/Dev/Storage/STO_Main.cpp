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

#include "STO_Main.h"
#include "4J_Storage.h"

CStorage InternalStorageManager;

CStorage::CStorage(void)
{
    m_SaveGame = CSaveGame();
    m_DLC = CDLC();
}

void CStorage::Init(int (*Func)(LPVOID, const C4JStorage::ESavingMessage, int), LPVOID lpParam, LPCSTR szGroupID) {}

void CStorage::Tick(void)
{
    m_DLC.Tick();
}

unsigned int CStorage::CRC(unsigned char *buf, int len)
{
    return ~UpdateCRC(0xFFFFFFFF, buf, len);
}

void CStorage::MakeCRCTable(void)
{
    for (int c = 0; c < 256; ++c)
    {
        unsigned int k = c;
        for (int n = 0; n < 8; ++n)
        {
            if ((k & 1) != 0)
            {
                k = (k >> 1) ^ 0xEDB88320;
            }
            else
            {
                k >>= 1;
            }
        }
        m_CRCTable[c] = k;
    }
    m_bHasCRCTable = true;
}

unsigned int CStorage::UpdateCRC(unsigned int crc, unsigned __int8 *buf, int len)
{
    if (!m_bHasCRCTable)
    {
        MakeCRCTable();
    }

    for (int c = 0; c < len; ++c)
    {
        crc = (crc >> 8) ^ m_CRCTable[(unsigned __int8)(buf[c] ^ crc)];
    }

    return crc;
}

void CStorage::DebugPrintf(const char *szFormat, ...)
{
    char buf[1024];

    va_list va;
    va_start(va, szFormat);

    vsnprintf(buf, 1024, szFormat, va);

    va_end(va);
}