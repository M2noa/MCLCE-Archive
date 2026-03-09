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

void CForceFeedback::Initialise(int iInputStateC, unsigned char ucMapC, unsigned char ucActionC, unsigned char ucMenuActionC)
{
    m_Effects = new FF_EFFECT[MAX_EFFECTS];
    m_EffectC = 0;

    LARGE_INTEGER qwTicksPerSec;
    QueryPerformanceFrequency(&qwTicksPerSec);

    m_TicksPerSecond = qwTicksPerSec.LowPart;
    m_unkBool = 0;
}

void CForceFeedback::RumbleEnable(int iQuadrant, bool bRumbleEnabled) {}

void CForceFeedback::CreateEffect(void)
{
    assert(m_EffectC < MAX_EFFECTS);

    FF_EFFECT *pEffect = &m_Effects[m_EffectC];
    pEffect->m_uiDurationMs = 5;
    pEffect->m_uiMagnitude = 500;
    pEffect->m_uiEffectType = 0;
    pEffect->dword18 = 10;
    pEffect->dword18 = 100;

    m_EffectC++;
}

void CForceFeedback::AddEffectInstance(unsigned int uiEffect)
{
    FF_EFFECT_INSTANCE *pEffectInst = new FF_EFFECT_INSTANCE();
    pEffectInst->m_Effect = &m_Effects[uiEffect];
    pEffectInst->m_uiInst = 0;

    m_EffectList.AddToTail(pEffectInst);
}

void CForceFeedback::SetEffectInstanceState(unsigned int uiEffectInstance, unsigned int uiState)
{
    int iCount = 0;
    LinkedList::_LL_NODE *pNode;
    for (pNode = m_EffectList.m_Head;; pNode = pNode->m_Next)
    {
        if ((iCount++) == uiEffectInstance)
            break;
    }
    pNode->GetDataAs<FF_EFFECT_INSTANCE>()->m_bIsActive = true;
}

void CForceFeedback::ProcessEventInstance(FF_EFFECT_INSTANCE *pEffectInst) {}

void CForceFeedback::PauseEventInstance(FF_EFFECT_INSTANCE *pEffectInst) {}

void CForceFeedback::AddRumble(unsigned int uiPad, WORD leftMotorSpeed, WORD rightMotorSpeed, float fSeconds)
{
    RUMBLE_EFFECT *pRumble = new RUMBLE_EFFECT();

    LARGE_INTEGER qwCurrentTime;
    QueryPerformanceCounter(&qwCurrentTime);

    pRumble->m_TimeLeft = (m_TicksPerSecond * fSeconds) + qwCurrentTime.QuadPart;
    pRumble->m_RumbleData.wLeftMotorSpeed = leftMotorSpeed;
    pRumble->m_RumbleData.wRightMotorSpeed = rightMotorSpeed;
    pRumble->m_Pad = uiPad;

    m_RumbleList.AddToTail(pRumble);
}

void CForceFeedback::Tick(void)
{
    for (LinkedList::_LL_NODE *pEffectNode = m_EffectList.m_Head; pEffectNode; pEffectNode = pEffectNode->m_Next)
    {
        if (pEffectNode->GetDataAs<FF_EFFECT_INSTANCE>()->m_bIsActive)
        {
            ProcessEventInstance(pEffectNode->GetDataAs<FF_EFFECT_INSTANCE>());
        }
    }

    LARGE_INTEGER qwCurrentTime;
    QueryPerformanceCounter(&qwCurrentTime);

    LinkedList::_LL_NODE *pRumbleNode = m_RumbleList.m_Head;
    while (pRumbleNode)
    {
        if (pRumbleNode->GetDataAs<RUMBLE_EFFECT>()->m_TimeLeft <= qwCurrentTime.QuadPart)
        {
            RUMBLE_EFFECT *pRumble = pRumbleNode->GetDataAs<RUMBLE_EFFECT>();

            pRumble->m_RumbleData.wLeftMotorSpeed = 0;
            pRumble->m_RumbleData.wRightMotorSpeed = 0;

            XInputSetState(pRumble->m_Pad, &pRumble->m_RumbleData);

            m_RumbleList.RemoveNode(pRumbleNode);

            LinkedList::_LL_NODE *pTemp = pRumbleNode;
            pRumbleNode = pRumbleNode->m_Next;

            delete pTemp->m_pvData;
            delete pTemp;
        }
        else
        {
            XInputSetState(pRumbleNode->GetDataAs<RUMBLE_EFFECT>()->m_Pad, &pRumbleNode->GetDataAs<RUMBLE_EFFECT>()->m_RumbleData);
            pRumbleNode = pRumbleNode->m_Next;
        }
    }
}