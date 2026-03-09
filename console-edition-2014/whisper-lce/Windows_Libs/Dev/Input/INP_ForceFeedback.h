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

#include "LinkedList.h"

#define MAX_EFFECTS 10

class CForceFeedback
{
public:
    enum FF_PARAMS
    {
        FF_PARAMS_RAMP,
        FF_PARAMS_PERIODIC
    };

    struct RUMBLE_EFFECT
    {
        DWORD m_Pad;
        uint64_t m_TimeLeft;
        XINPUT_VIBRATION m_RumbleData;
        WORD unk;
    };

    struct FF_EFFECT
    {
        unsigned int m_uiEffectType;
        unsigned int m_uiDurationMs;
        BYTE gap8[4];
        unsigned int m_uiMagnitude;
        BYTE gap10[8];
        DWORD dword18;
        DWORD dword1C;
    };

    struct FF_EFFECT_INSTANCE
    {
        bool m_bIsActive;
        unsigned int m_uiInst;
        CForceFeedback::FF_EFFECT *m_Effect;
    };

    void Initialise(int iInputStateC, unsigned char ucMapC, unsigned char ucActionC, unsigned char ucMenuActionC);
    void RumbleEnable(int iQuadrant, bool bRumbleEnabled);
    void CreateEffect(void);
    void AddEffectInstance(unsigned int uiEffect);
    void SetEffectInstanceState(unsigned int uiEffectInstance, unsigned int uiState);

    void ProcessEventInstance(FF_EFFECT_INSTANCE *pEffectInst);
    void PauseEventInstance(FF_EFFECT_INSTANCE *pEffectInst);

    void AddRumble(unsigned int uiPad, WORD leftMotorSpeed, WORD rightMotorSpeed, float fSeconds);

    void Tick(void);

    LinkedList m_EffectList;
    FF_EFFECT *m_Effects;
    int m_EffectC;
    LinkedList m_RumbleList;
    float m_TicksPerSecond;
    bool m_unkBool;
};