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

#include "INP_ForceFeedback.h"
#include "INP_Keyboard.h"

#define MAX_JOYPADS 4

class CInput
{
public:
    struct JOYPAD
    {
        bool m_bIsDisabled;
        bool m_bIsConnected;
        bool m_bWasConnected;
        bool m_bHasConnected;

        unsigned int m_uiButtons;
        unsigned int m_uiOldButtons;
        int m_uiButtonsPressed;
        int m_uiButtonsReleased;

        int m_iRightThumbX;
        int m_iRightThumbY;
        int m_iLeftThumbX;
        int m_iLeftThumbY;

        DWORD m_iNormalizedRightThumbX;
        DWORD m_iNormalizedRightThumbY;
        DWORD m_iNormalizedLeftThumbX;
        DWORD m_iNormalizedLeftThumbY;

        float m_fNormalizedRightThumbX;
        float m_fNormalizedRightThumbY;
        float m_fNormalizedLeftThumbX;
        float m_fNormalizedLeftThumbY;

        unsigned char m_ucLeftTriggerState;
        unsigned char m_ucRightTriggerState;

        float *m_pfLeftThumbXAxisMap;
        float *m_pfLeftThumbYAxisMap;
        float *m_pfRightThumbXAxisMap;
        float *m_pfRightThumbYAxisMap;

        float m_fSensitivity;

        BYTE *m_pucLeftTriggerAxisMap;
        BYTE *m_pucRightTriggerAxisMap;

        unsigned char m_ucMappingValue;
        unsigned char m_ucInputStateIndex;
        XINPUT_STATE *m_pInputStates;
    };

    struct JOYPADS
    {
    };

    struct TimeInfo
    {
        LARGE_INTEGER m_qwStartTime;
        LARGE_INTEGER m_qwTotalHoldTicks;
        float m_fTotalHoldTime;
        float m_fLastRepeatTime;
        bool m_bInitialPressHandled;
    };

    CInput();

    void Initialise(int iInputStateC, unsigned char ucMapC, unsigned char ucActionC, unsigned char ucMenuActionC);
    void Tick(void);

    void SetJoypadValues(JOYPAD *pThisPad);
    void SetDeadzoneAndMovementRange(unsigned int uiDeadzone, unsigned int uiMovementRangeMax);
    void SetJoypadSensitivity(int iPad, float fSensitivity);

    void SetGameJoypadMaps(unsigned char ucMap, unsigned char ucAction, unsigned int uiActionVal);
    unsigned int GetGameJoypadMaps(unsigned char ucMap, unsigned char ucAction);
    void SetJoypadMapVal(int iPad, unsigned char ucMap);
    unsigned char GetJoypadMapVal(int iPad);

    void SetJoypadStickAxisMap(int iPad, unsigned int uiFrom, unsigned int uiTo);
    void SetJoypadStickTriggerMap(int iPad, unsigned int uiFrom, unsigned int uiTo);

    bool IsPadConnected(int iPad);
    void SetSigninJoypadMask(unsigned int mask);
    unsigned int GetValue(int iPad, unsigned char ucAction, bool bRepeat = false);
    bool IsSet(int iPad, unsigned char ucAction);

    bool ButtonPressed(int iPad, unsigned char ucAction = 255); // toggled
    bool ButtonReleased(int iPad, unsigned char ucAction);      // toggled
    bool ButtonDown(int iPad, unsigned char ucAction = 255);    // button held down

    float GetJoypadStick_Menu_LX(unsigned char ucPad);
    float GetJoypadStick_Menu_LY(unsigned char ucPad);
    float GetJoypadStick_Menu_RX(unsigned char ucPad);
    float GetJoypadStick_Menu_RY(unsigned char ucPad);
    unsigned char GetJoypadLTrigger_Menu(unsigned char ucPad);
    unsigned char GetJoypadRTrigger_Menu(unsigned char ucPad);

    float GetJoypadStick_LX(int iPad, bool bCheckMenuDisplay = true);
    float GetJoypadStick_LY(int iPad, bool bCheckMenuDisplay = true);
    float GetJoypadStick_RX(int iPad, bool bCheckMenuDisplay = true);
    float GetJoypadStick_RY(int iPad, bool bCheckMenuDisplay = true);
    unsigned char GetJoypadLTrigger(int iPad, bool bCheckMenuDisplay = true);
    unsigned char GetJoypadRTrigger(int iPad, bool bCheckMenuDisplay = true);

    void SetMenuDisplayed(int iPad, bool bVal);

    void SetKeyRepeatRate(float fRepeatDelaySecs, float fRepeatRateSecs);
    void SetDebugSequence(const char *chSequenceA, int (*Func)(LPVOID), LPVOID lpParam);
    FLOAT GetIdleSeconds(int iPad);

    bool UpdateJoypads(void);
    void ClearJoypadValues(JOYPAD *pThisPad);

    void InitTime(void);
    void GetStartTime(int iPad, int iKey);
    void UpdateTime(int iPad, int iKey);

    BYTE gap0[16];

    JOYPAD m_Joypads[MAX_JOYPADS];

    int m_iDeadzone;
    int m_iMovementRangeMax;
    int m_iEffectiveRange;
    int m_iHalfRange;
    float m_fEffectiveRange;

    unsigned char m_ucInputStateC;
    unsigned char m_ucJoypadMapC;
    unsigned char m_ucJoypadMapActionC;
    unsigned char m_ucMenuActionC;

    unsigned int **m_JoypadMap;

    bool m_bJoypadMapArrayIsSetup;
    unsigned int m_uiSigninJoypadMask;
    bool m_bIsMenuDisplayed[MAX_JOYPADS];

    CForceFeedback m_ForceFeedback;
    CKeyboard m_Keyboard;

    float m_fTickToSeconds;
    float m_fRepeatDelaySecs;
    float m_fRepeatRateSecs;

    TimeInfo m_Timers[MAX_JOYPADS][24];
    LARGE_INTEGER m_LastActivityTime[MAX_JOYPADS];

    char *m_sDebugSequenceName;
    unsigned int m_uiDebugSequenceIndex;
    int (*m_pDebugSequenceFn)(void *);
    LPVOID m_pDebugSequenceParam;
};

// Singleton
extern CInput InternalInputManager;