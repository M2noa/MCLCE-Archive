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

#include "INP_Keyboard.h"

void CKeyboard::Tick(void) {}

EKeyboardResult CKeyboard::RequestKeyboard(LPCWSTR Title, LPCWSTR Text, DWORD dwPad, UINT uiMaxChars, int (*Func)(LPVOID, const bool), LPVOID lpParam,
                                           C_4JInput::EKeyboardMode eMode)
{
    ClearGlobalText();
    SeedEditBox();
    Func(lpParam, C_4JInput::EKeyboardMode_Numeric);

    return EKeyboard_ResultAccept;
}

void CKeyboard::GetText(uint16_t *UTF16String)
{
    uint16_t *keyString = GetGlobalText();
    for (unsigned int index = 0; keyString[index] && index < 0x200; index++)
    {
        UTF16String[index] = keyString[index];
    }
}