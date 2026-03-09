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

#include "stdafx.h"
#include "Renderer.h"

#include <cstring>

const float *Renderer::MatrixGet(int type)
{
    Context &c = getContext();
    const int depth = c.stackPos[type];
    const DirectX::XMMATRIX &matrix = c.matrixStacks[type][depth];
    return &matrix.r[0].m128_f32[0];
}

void Renderer::MatrixMode(int type)
{
    Context &c = getContext();
    assert(type >= 0);
    assert(type < STACK_TYPES);
    c.stackType = type;
}

void Renderer::MatrixMult(float *mat)
{
    DirectX::XMMATRIX matrix;
    std::memcpy(&matrix, mat, sizeof(matrix));
    MultWithStack(matrix);
}

void Renderer::MatrixOrthogonal(float left, float right, float bottom, float top, float zNear, float zFar)
{
    const DirectX::XMMATRIX matrix = DirectX::XMMatrixOrthographicOffCenterRH(left, right, bottom, top, zNear, zFar);
    MultWithStack(matrix);
}

void Renderer::MatrixPerspective(float fovy, float aspect, float zNear, float zFar)
{
    const float fovRadians = fovy * (PI / 180.0f);
    const DirectX::XMMATRIX matrix = DirectX::XMMatrixPerspectiveFovRH(fovRadians, aspect, zNear, zFar);
    MultWithStack(matrix);
}

void Renderer::MatrixPop()
{
    Context &c = getContext();

    assert(c.stackPos[c.stackType] > 0);

    const int mode = c.stackType;
    --c.stackPos[mode];
    c.matrixDirty[mode] = true;
}

void Renderer::MatrixPush()
{
    Context &c = getContext();
    
    assert(c.stackPos[c.stackType] < (STACK_SIZE - 1));

    const int mode = c.stackType;
    const int depth = c.stackPos[mode];
    c.matrixStacks[mode][depth + 1] = c.matrixStacks[mode][depth];
    ++c.stackPos[mode];
}

void Renderer::MatrixRotate(float angle, float x, float y, float z)
{
    const DirectX::XMVECTOR axis = DirectX::XMVectorSet(x, y, z, 0.0f);
    const DirectX::XMMATRIX matrix = DirectX::XMMatrixRotationAxis(axis, angle);
    MultWithStack(matrix);
}

void Renderer::MatrixScale(float x, float y, float z)
{
    const DirectX::XMMATRIX matrix = DirectX::XMMatrixScaling(x, y, z);
    MultWithStack(matrix);
}

void Renderer::MatrixSetIdentity()
{
    Context &c = getContext();
    const int mode = c.stackType;
    const int depth = c.stackPos[mode];
    c.matrixStacks[mode][depth] = DirectX::XMMatrixIdentity();
    c.matrixDirty[mode] = true;
}

void Renderer::MatrixTranslate(float x, float y, float z)
{
    const DirectX::XMMATRIX matrix = DirectX::XMMatrixTranslation(x, y, z);
    MultWithStack(matrix);
}

void Renderer::MultWithStack(DirectX::XMMATRIX matrix)
{
    Context &c = getContext();
    const int mode = c.stackType;
    const int depth = c.stackPos[mode];
    DirectX::XMMATRIX &current = c.matrixStacks[mode][depth];
    current = DirectX::XMMatrixMultiply(matrix, current);
    c.matrixDirty[mode] = true;
}

void Renderer::Set_matrixDirty()
{
    Context &c = getContext();
    const DirectX::XMMATRIX identity = DirectX::XMMatrixIdentity();

    c.matrixStacks[MATRIX_MODE_MODELVIEW][0] = identity;
    c.matrixStacks[MATRIX_MODE_MODELVIEW_PROJECTION][0] = identity;
    c.matrixStacks[MATRIX_MODE_MODELVIEW_TEXTURE][0] = identity;
    c.matrixStacks[MATRIX_MODE_MODELVIEW_CBUFF][0] = identity;

    c.matrixDirty[MATRIX_MODE_MODELVIEW] = true;
    c.matrixDirty[MATRIX_MODE_MODELVIEW_PROJECTION] = true;
    c.matrixDirty[MATRIX_MODE_MODELVIEW_TEXTURE] = true;
    c.matrixDirty[MATRIX_MODE_MODELVIEW_CBUFF] = true;

    activeVertexType = -1;
    activePixelType = -1;
}
