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

LinkedList::LinkedList()
{
    this->m_NodeC = 0;
    this->m_Head = nullptr;
    this->m_Tail = nullptr;
}

void LinkedList::AddToHead(void *pvData)
{
    _LL_NODE *pNode = new _LL_NODE();
    pNode->m_pvData = pvData;

    if (!m_Tail)
    {
        m_Tail = pNode;
    }

    if (m_Head)
    {
        m_Head->m_Prev = pNode;
        pNode->m_Next = m_Head;
    }

    m_Head = pNode;
    m_NodeC++;
}

void LinkedList::AddToTail(void *pvData)
{
    _LL_NODE *pNode = new _LL_NODE();
    pNode->m_pvData = pvData;

    if (!m_Head)
    {
        m_Head = pNode;
    }

    if (m_Tail)
    {
        m_Tail->m_Next = pNode;
        pNode->m_Prev = m_Tail;
    }

    pNode->m_Next = nullptr;
    pNode->m_Prev = m_Tail;

    m_Tail = pNode;
    m_NodeC++;
}

void LinkedList::RemoveNode(_LL_NODE *pNodeToRemove)
{
    _LL_NODE *pCurrentNode;
    for (pCurrentNode = m_Head; pCurrentNode != pNodeToRemove; pCurrentNode = pCurrentNode->m_Next)
    {
        ;
    }

    if (pCurrentNode == m_Head)
    {
        m_Head = m_Head->m_Next;
    }
    else
    {
        pCurrentNode->m_Prev->m_Next = pCurrentNode->m_Next;

        if (pCurrentNode->m_Next)
        {
            pCurrentNode->m_Next->m_Prev = pCurrentNode->m_Prev;
        }
    }

    m_NodeC--;
}

LinkedList::_LL_NODE *LinkedList::RemoveHeadNode()
{
    _LL_NODE *pHeadNode = m_Head;

    if (m_NodeC > 0)
    {
        m_Head = m_Head->m_Next;
        m_NodeC--;
    }

    return pHeadNode;
}

void LinkedList::ClearList()
{
    while (true)
    {
        _LL_NODE *pHeadNode = RemoveHeadNode();
        if (!pHeadNode)
        {
            break;
        }
        delete pHeadNode;
    }
}