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

#include "4J_Input.h"
#include "INP_Keyboard.h"
#include "INP_Main.h"

C_4JInput InputManager;
CKeyboard InternalKeyboard;

void C_4JInput::Initialise(int iInputStateC, unsigned char ucMapC, unsigned char ucActionC, unsigned char ucMenuActionC)
{
    InternalInputManager.Initialise(iInputStateC, ucMapC, ucActionC, ucMenuActionC);
}

void C_4JInput::Tick(void)
{
    InternalInputManager.Tick();
}

void C_4JInput::SetDeadzoneAndMovementRange(unsigned int uiDeadzone, unsigned int uiMovementRangeMax)
{
    InternalInputManager.SetDeadzoneAndMovementRange(uiDeadzone, uiMovementRangeMax);
}

void C_4JInput::SetGameJoypadMaps(unsigned char ucMap, unsigned char ucAction, unsigned int uiActionVal)
{
    InternalInputManager.SetGameJoypadMaps(ucMap, ucAction, uiActionVal);
}

unsigned int C_4JInput::GetGameJoypadMaps(unsigned char ucMap, unsigned char ucAction)
{
    return InternalInputManager.GetGameJoypadMaps(ucMap, ucAction);
}

void C_4JInput::SetJoypadMapVal(int iPad, unsigned char ucMap)
{
    InternalInputManager.SetJoypadMapVal(iPad, ucMap);
}

unsigned char C_4JInput::GetJoypadMapVal(int iPad)
{
    return InternalInputManager.GetJoypadMapVal(iPad);
}

void C_4JInput::SetJoypadSensitivity(int iPad, float fSensitivity)
{
    InternalInputManager.SetJoypadSensitivity(iPad, fSensitivity);
}

unsigned int C_4JInput::GetValue(int iPad, unsigned char ucAction, bool bRepeat)
{
    return InternalInputManager.GetValue(iPad, ucAction, bRepeat);
}

bool C_4JInput::ButtonPressed(int iPad, unsigned char ucAction)
{
    return InternalInputManager.ButtonPressed(iPad, ucAction);
}

bool C_4JInput::ButtonReleased(int iPad, unsigned char ucAction)
{
    return InternalInputManager.ButtonReleased(iPad, ucAction);
}

bool C_4JInput::ButtonDown(int iPad, unsigned char ucAction)
{
    return InternalInputManager.ButtonDown(iPad, ucAction);
}

void C_4JInput::SetJoypadStickAxisMap(int iPad, unsigned int uiFrom, unsigned int uiTo)
{
    InternalInputManager.SetJoypadStickAxisMap(iPad, uiFrom, uiTo);
}

void C_4JInput::SetJoypadStickTriggerMap(int iPad, unsigned int uiFrom, unsigned int uiTo)
{
    InternalInputManager.SetJoypadStickTriggerMap(iPad, uiFrom, uiTo);
}

void C_4JInput::SetKeyRepeatRate(float fRepeatDelaySecs, float fRepeatRateSecs)
{
    InternalInputManager.SetKeyRepeatRate(fRepeatDelaySecs, fRepeatRateSecs);
}

void C_4JInput::SetDebugSequence(const char *chSequenceA, int (*Func)(LPVOID), LPVOID lpParam)
{
    InternalInputManager.SetDebugSequence(chSequenceA, Func, lpParam);
}

FLOAT C_4JInput::GetIdleSeconds(int iPad)
{
    return InternalInputManager.GetIdleSeconds(iPad);
}

bool C_4JInput::IsPadConnected(int iPad)
{
    return InternalInputManager.IsPadConnected(iPad);
}

float C_4JInput::GetJoypadStick_LX(int iPad, bool bCheckMenuDisplay)
{
    return InternalInputManager.GetJoypadStick_LX(iPad, bCheckMenuDisplay);
}

float C_4JInput::GetJoypadStick_LY(int iPad, bool bCheckMenuDisplay)
{
    return InternalInputManager.GetJoypadStick_LY(iPad, bCheckMenuDisplay);
}

float C_4JInput::GetJoypadStick_RX(int iPad, bool bCheckMenuDisplay)
{
    return InternalInputManager.GetJoypadStick_RX(iPad, bCheckMenuDisplay);
}

float C_4JInput::GetJoypadStick_RY(int iPad, bool bCheckMenuDisplay)
{
    return InternalInputManager.GetJoypadStick_RY(iPad, bCheckMenuDisplay);
}

unsigned char C_4JInput::GetJoypadLTrigger(int iPad, bool bCheckMenuDisplay)
{
    return InternalInputManager.GetJoypadLTrigger(iPad, bCheckMenuDisplay);
}

unsigned char C_4JInput::GetJoypadRTrigger(int iPad, bool bCheckMenuDisplay)
{
    return InternalInputManager.GetJoypadRTrigger(iPad, bCheckMenuDisplay);
}

void C_4JInput::SetMenuDisplayed(int iPad, bool bVal)
{
    InternalInputManager.SetMenuDisplayed(iPad, bVal);
}

EKeyboardResult C_4JInput::RequestKeyboard(LPCWSTR Title, LPCWSTR Text, DWORD dwPad, UINT uiMaxChars, int (*Func)(LPVOID, const bool), LPVOID lpParam,
                                           C_4JInput::EKeyboardMode eMode)
{
    return InternalKeyboard.RequestKeyboard(Title, Text, dwPad, uiMaxChars, Func, lpParam, eMode);
}

void C_4JInput::GetText(uint16_t *UTF16String)
{
    InternalKeyboard.GetText(UTF16String);
}

bool C_4JInput::VerifyStrings(WCHAR **pwStringA, int iStringC, int (*Func)(LPVOID, STRING_VERIFY_RESPONSE *), LPVOID lpParam)
{
    return true;
}

void C_4JInput::CancelQueuedVerifyStrings(int (*Func)(LPVOID, STRING_VERIFY_RESPONSE *), LPVOID lpParam) {}

void C_4JInput::CancelAllVerifyInProgress(void) {}

// bool C_4JInput::InputDetected(DWORD dwUserIndex,WCHAR *pwchInput) {}