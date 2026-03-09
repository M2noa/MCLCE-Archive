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

#include <cstdint>
#include <cstring>
#include <new>

Renderer::CommandBuffer::CommandBuffer(bool full)
    : m_vertexBuffer(NULL)
    , m_vertexData(NULL)
    , m_vertexDataLength(0)
    , m_commands()
    , m_allocated(0x1000)
    , isActive(full ? 1 : 0)
{
    m_vertexData = std::malloc(m_allocated);
    EnterCriticalSection(&Renderer::totalAllocCS);
    Renderer::totalAlloc += static_cast<int>(m_allocated);
    LeaveCriticalSection(&Renderer::totalAllocCS);
}

Renderer::CommandBuffer::~CommandBuffer()
{
    if (m_vertexBuffer)
        m_vertexBuffer->Release();

    std::free(m_vertexData);

    EnterCriticalSection(&Renderer::totalAllocCS);
    Renderer::totalAlloc -= static_cast<int>(m_allocated);
    LeaveCriticalSection(&Renderer::totalAllocCS);
}

void Renderer::CommandBuffer::AddMatrix(const float *matrix)
{
    Command command = {};
    command.m_command_type = COMMAND_ADD_MATRIX;
    std::memcpy(command.add_matrix.m_matrix, matrix, sizeof(command.add_matrix.m_matrix));
    m_commands.push_back(command);
}

void Renderer::CommandBuffer::AddVertices(unsigned int stride, unsigned int count, void *dataIn, Renderer::Context &c)
{
    PROFILER_SCOPE("Renderer::CommandBuffer::AddVertices", "AddVertices", MP_ORANGE);

    if (c.matrixDirty[MATRIX_MODE_MODELVIEW_CBUFF])
    {
        AddMatrix(InternalRenderManager.MatrixGet(MATRIX_MODE_MODELVIEW_CBUFF));
        c.matrixDirty[MATRIX_MODE_MODELVIEW_CBUFF] = false;
    }

    const std::uint64_t vertexOffset = m_vertexDataLength;
    const std::uint64_t copySize = std::uint64_t(stride) * std::uint64_t(count);

    Command command = {};
    command.m_command_type = COMMAND_ADD_VERTICES;
    command.add_vertices.m_vertex_index_start = static_cast<unsigned int>(vertexOffset);
    command.add_vertices.m_vertex_count = count;

    m_vertexDataLength = vertexOffset + copySize;
    if (m_vertexDataLength > m_allocated)
    {
        EnterCriticalSection(&Renderer::totalAllocCS);
        Renderer::totalAlloc -= static_cast<int>(m_allocated);
        LeaveCriticalSection(&Renderer::totalAllocCS);

        m_allocated = ((m_vertexDataLength + (0x1000 - 1)) & ~(0x1000 - 1));
        m_vertexData = std::realloc(m_vertexData, m_allocated);

        EnterCriticalSection(&Renderer::totalAllocCS);
        Renderer::totalAlloc += static_cast<int>(m_allocated);
        LeaveCriticalSection(&Renderer::totalAllocCS);
    }

    const std::size_t byteCount = std::size_t(stride) * std::size_t(count);
    std::memcpy(static_cast<std::uint8_t*>(m_vertexData) + vertexOffset, dataIn, byteCount);
    m_commands.push_back(command);
}

void Renderer::CommandBuffer::BindTexture(int idx)
{
    Command command = {};
    command.m_command_type = COMMAND_BIND_TEXTURE;
    command.bind_texture.m_texture_index = idx;
    m_commands.push_back(command);
}

bool Renderer::CBuffCall(int index, bool full)
{
    EnterCriticalSection(&m_commandBufferCS);

    bool result = false;
    const int commandIndex = m_vertexIdxToBufferIdx[index];
    if (commandIndex >= 0)
    {
        Renderer::Context &c = getContext();
        const std::uint8_t vertexType = m_commandVertexTypes[commandIndex];
        const std::uint8_t primitiveType = m_commandPrimitiveTypes[commandIndex];

        if (full)
        {
            if (c.matrixDirty[MATRIX_MODE_MODELVIEW])
            {
                D3D11_MAPPED_SUBRESOURCE mappedMatrix0 = {};
                c.m_pDeviceContext->Map(c.m_modelViewMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedMatrix0);
                std::memcpy(mappedMatrix0.pData, MatrixGet(MATRIX_MODE_MODELVIEW), sizeof(DirectX::XMMATRIX));
                c.m_pDeviceContext->Unmap(c.m_modelViewMatrix, 0);
                c.matrixDirty[MATRIX_MODE_MODELVIEW] = false;
            }

            if (c.matrixDirty[MATRIX_MODE_MODELVIEW_PROJECTION])
            {
                D3D11_MAPPED_SUBRESOURCE mappedMatrix2 = {};
                c.m_pDeviceContext->Map(c.m_projectionMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedMatrix2);
                std::memcpy(mappedMatrix2.pData, MatrixGet(MATRIX_MODE_MODELVIEW_PROJECTION), sizeof(DirectX::XMMATRIX));
                c.m_pDeviceContext->Unmap(c.m_projectionMatrix, 0);
                c.matrixDirty[MATRIX_MODE_MODELVIEW_PROJECTION] = false;
            }

            if (c.matrixDirty[MATRIX_MODE_MODELVIEW_TEXTURE])
            {
                D3D11_MAPPED_SUBRESOURCE mappedMatrix3 = {};
                c.m_pDeviceContext->Map(c.m_textureMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedMatrix3);
                std::memcpy(mappedMatrix3.pData, MatrixGet(MATRIX_MODE_MODELVIEW_TEXTURE), sizeof(DirectX::XMMATRIX));
                c.m_pDeviceContext->Unmap(c.m_textureMatrix, 0);
                c.matrixDirty[MATRIX_MODE_MODELVIEW_TEXTURE] = false;
            }

            UpdateFogState();
            UpdateLightingState();
            UpdateViewportState();
            UpdateTexGenState();

            if (vertexType != activeVertexType)
            {
                c.m_pDeviceContext->VSSetShader(vertexShaderTable[vertexType], NULL, 0);
                c.m_pDeviceContext->IASetInputLayout(inputLayoutTable[vertexType]);
                activeVertexType = vertexType;
            }

            int pixelType = 0;
            if (static_cast<int>(c.forcedLOD) > -1)
            {
                const float forcedLod[4] = {static_cast<float>(static_cast<int>(c.forcedLOD)), 0.0f, 0.0f, 0.0f};
                D3D11_MAPPED_SUBRESOURCE mappedAux4 = {};
                c.m_pDeviceContext->Map(c.m_forcedLODBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedAux4);
                std::memcpy(mappedAux4.pData, forcedLod, sizeof(forcedLod));
                c.m_pDeviceContext->Unmap(c.m_forcedLODBuffer, 0);
                pixelType = C4JRender::PIXEL_SHADER_TYPE_FORCELOD;
            }

            if (static_cast<DWORD>(pixelType) != activePixelType)
            {
                c.m_pDeviceContext->PSSetShader(pixelShaderTable[pixelType], NULL, 0);
                activePixelType = pixelType;
            }

            c.m_pDeviceContext->IASetPrimitiveTopology(g_topologies[primitiveType]);

            ID3D11Buffer *indexBuffer = NULL;
            if (primitiveType == C4JRender::PRIMITIVE_TYPE_QUAD_LIST)
                indexBuffer = quadIndexBuffer;
            else if (primitiveType == C4JRender::PRIMITIVE_TYPE_TRIANGLE_FAN)
                indexBuffer = fanIndexBuffer;

            c.m_pDeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);
        }

        m_commandBuffers[commandIndex]->Render(static_cast<C4JRender::eVertexType>(vertexType), c, primitiveType);

        if (full)
        {
            MultWithStack(m_commandMatrices[commandIndex]);
            c.matrixStacks[MATRIX_MODE_MODELVIEW_CBUFF][0] = DirectX::XMMatrixIdentity();
            c.matrixDirty[MATRIX_MODE_MODELVIEW_CBUFF] = true;
        }

        result = true;
    }

    LeaveCriticalSection(&m_commandBufferCS);
    return result;
}

void Renderer::CBuffClear(int index)
{
    EnterCriticalSection(&m_commandBufferCS);

    const int internalIndex = m_vertexIdxToBufferIdx[index];
    if (internalIndex >= 0)
    {
        DeleteInternalBuffer(internalIndex);
        m_vertexIdxToBufferIdx[index] = static_cast<std::int16_t>(-2);
    }

    LeaveCriticalSection(&m_commandBufferCS);
}

int Renderer::CBuffCreate(int count)
{
    EnterCriticalSection(&m_commandBufferCS);

    int first = reservedRendererDword1;
    if (first < NUM_COMMAND_HANDLES)
    {
        int probe = first;
        int end = first + count;
        while (true)
        {
            assert(first < NUM_COMMAND_HANDLES);

            int cursor = probe;
            while (cursor < end && cursor < NUM_COMMAND_HANDLES && m_vertexIdxToBufferIdx[cursor] == static_cast<std::int16_t>(-1))
            {
                ++cursor;
            }

            if (cursor >= end)
                break;

            ++first;
            ++probe;
            ++end;
            if (first >= NUM_COMMAND_HANDLES || end > NUM_COMMAND_HANDLES)
            {
                first = -1;
                break;
            }
        }

        if (first >= 0)
        {
            const int allocationEnd = first + count;
            for (int i = first; i < allocationEnd; ++i)
                m_vertexIdxToBufferIdx[i] = static_cast<std::int16_t>(-2);

            if (reservedRendererByte1)
                reservedRendererDword1 = allocationEnd;
        }
    }
    else
    {
        first = -1;
    }

    LeaveCriticalSection(&m_commandBufferCS);
    return first;
}

void Renderer::CBuffDeferredModeEnd()
{
    Renderer::Context &c = getContext();
    if (!c.deferredModeEnabled)
        return;

    EnterCriticalSection(&m_commandBufferCS);
    c.deferredModeEnabled = false;

    for (std::vector<Renderer::DeferredCBuff>::const_iterator it = c.deferredBuffers.begin(); it != c.deferredBuffers.end(); ++it)
    {
        const Renderer::DeferredCBuff &deferred = *it;
        const int existingIndex = m_vertexIdxToBufferIdx[deferred.m_vertex_index];
        if (existingIndex >= 0)
            DeleteInternalBuffer(existingIndex);

        if (static_cast<int>(m_currentCommandBuffer + m_numBuffersToDeallocate + 10) > MAX_COMMAND_BUFFERS)
            DebugBreak();

        const int internalSlot = m_currentCommandBuffer;
        ++m_currentCommandBuffer;

        m_vertexIdxToBufferIdx[deferred.m_vertex_index] = static_cast<std::int16_t>(internalSlot);
        m_bufferIdxToVertexIdx[internalSlot] = deferred.m_vertex_index;
        m_commandVertexTypes[internalSlot] = static_cast<std::uint8_t>(deferred.m_vertex_type);
        m_commandPrimitiveTypes[internalSlot] = static_cast<std::uint8_t>(deferred.m_primitive_type);
        m_commandBuffers[internalSlot] = deferred.m_command_buf;
        m_commandMatrices[internalSlot] = deferred.m_matrix;
    }

    c.deferredBuffers.clear();
    LeaveCriticalSection(&m_commandBufferCS);
}

void Renderer::CBuffDeferredModeStart()
{
    getContext().deferredModeEnabled = true;
}

void Renderer::CBuffDelete(int first, int count)
{
    EnterCriticalSection(&m_commandBufferCS);

    const int end = first + count;
    for (int i = first; i < end; ++i)
    {
        const int internalIndex = m_vertexIdxToBufferIdx[i];
        if (internalIndex >= 0)
            DeleteInternalBuffer(internalIndex);

        m_vertexIdxToBufferIdx[i] = static_cast<std::int16_t>(-1);
    }

    LeaveCriticalSection(&m_commandBufferCS);
}

void Renderer::CBuffEnd()
{
    Renderer::Context &c = getContext();

    assert(c.stackType == MATRIX_MODE_MODELVIEW_CBUFF);
    assert(c.stackPos[MATRIX_MODE_MODELVIEW_CBUFF] == 0);

    EnterCriticalSection(&m_commandBufferCS);

    if (c.deferredModeEnabled)
    {
        Renderer::DeferredCBuff deferred;
        deferred.m_command_buf = c.commandBuffer;
        deferred.m_vertex_index = c.recordingBufferIndex;
        deferred.m_vertex_type = c.recordingVertexType;
        deferred.m_primitive_type = c.recordingPrimitiveType;
        deferred.m_matrix = c.matrixStacks[MATRIX_MODE_MODELVIEW_CBUFF][0];
        c.deferredBuffers.push_back(deferred);
    }
    else
    {
        const int existingIndex = m_vertexIdxToBufferIdx[c.recordingBufferIndex];
        if (existingIndex >= 0)
            DeleteInternalBuffer(existingIndex);

        if (static_cast<int>(m_currentCommandBuffer + m_numBuffersToDeallocate + 10) > MAX_COMMAND_BUFFERS)
            DebugBreak();

        const int internalSlot = m_currentCommandBuffer;
        ++m_currentCommandBuffer;

        m_vertexIdxToBufferIdx[c.recordingBufferIndex] = static_cast<std::int16_t>(internalSlot);
        m_bufferIdxToVertexIdx[internalSlot] = c.recordingBufferIndex;
        m_commandVertexTypes[internalSlot] = static_cast<std::uint8_t>(c.recordingVertexType);
        m_commandPrimitiveTypes[internalSlot] = static_cast<std::uint8_t>(c.recordingPrimitiveType);
        m_commandBuffers[internalSlot] = c.commandBuffer;
        m_commandMatrices[internalSlot] = c.matrixStacks[MATRIX_MODE_MODELVIEW_CBUFF][0];
    }

    c.stackType = MATRIX_MODE_MODELVIEW;
    c.commandBuffer->EndRecording(m_pDevice);
    c.commandBuffer = NULL;

    LeaveCriticalSection(&m_commandBufferCS);
}

void Renderer::CBuffLockStaticCreations()
{
    reservedRendererByte1 = 0;
}

int Renderer::CBuffSize(int index)
{
    if (index == -1)
        return totalAlloc < 0 ? 0 : totalAlloc;

    unsigned int size = 0;
    EnterCriticalSection(&m_commandBufferCS);
    const int commandIndex = m_vertexIdxToBufferIdx[index];
    if (commandIndex >= 0)
        size = static_cast<unsigned int>(m_commandBuffers[commandIndex]->GetAllocated());
    LeaveCriticalSection(&m_commandBufferCS);
    return size;
}

void Renderer::CBuffStart(int index, bool full)
{
    Renderer::Context &c = getContext();
    c.commandBuffer = new (std::nothrow) Renderer::CommandBuffer(full);
    c.recordingBufferIndex = index;

    assert(c.stackType == MATRIX_MODE_MODELVIEW);

    c.stackType = MATRIX_MODE_MODELVIEW_CBUFF;
    c.stackPos[MATRIX_MODE_MODELVIEW_CBUFF] = 0;
    c.matrixStacks[MATRIX_MODE_MODELVIEW_CBUFF][0] = DirectX::XMMatrixIdentity();
    c.matrixDirty[MATRIX_MODE_MODELVIEW_CBUFF] = true;
}

void Renderer::CBuffTick()
{
    EnterCriticalSection(&m_commandBufferCS);

    int completedDeletes = 0;
    if (m_numBuffersToDeallocate > 0)
    {
        int deleteIdx = MAX_COMMAND_BUFFERS - 1;
        do
        {
            Renderer::CommandBuffer *buffer = m_commandBuffers[deleteIdx];
            if (buffer)
                delete buffer;
            m_commandBuffers[deleteIdx] = NULL;

            if (--m_numBuffersToDeallocate == 0)
            {
                ++completedDeletes;
                --deleteIdx;
            }
            else
            {
                m_commandBuffers[deleteIdx] = m_commandBuffers[(MAX_COMMAND_BUFFERS - 1) - static_cast<int>(m_numBuffersToDeallocate)];
            }
        } while (completedDeletes < static_cast<int>(m_numBuffersToDeallocate));
    }

    LeaveCriticalSection(&m_commandBufferCS);
}

void Renderer::DeleteInternalBuffer(int index)
{
    EnterCriticalSection(&m_commandBufferCS);

    ++m_numBuffersToDeallocate;
    const int index_delete = MAX_COMMAND_BUFFERS - static_cast<int>(m_numBuffersToDeallocate);

    m_commandBuffers[index_delete] = m_commandBuffers[index];
    m_commandMatrices[index_delete] = m_commandMatrices[index];

    if (m_currentCommandBuffer-- != 1)
    {
        const int mappedFrom = m_currentCommandBuffer;

        m_commandBuffers[index] = m_commandBuffers[mappedFrom];
        m_commandMatrices[index] = m_commandMatrices[mappedFrom];
        m_commandVertexTypes[index] = m_commandVertexTypes[mappedFrom];
        m_commandPrimitiveTypes[index] = m_commandPrimitiveTypes[mappedFrom];

        const int commandIndex = m_bufferIdxToVertexIdx[mappedFrom];
        m_vertexIdxToBufferIdx[commandIndex] = static_cast<std::int16_t>(index);
        m_bufferIdxToVertexIdx[index] = commandIndex;
    }

    LeaveCriticalSection(&m_commandBufferCS);
}

void Renderer::CommandBuffer::EndRecording(ID3D11Device *device)
{
    if (m_vertexDataLength != 0)
    {
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = static_cast<UINT>(m_vertexDataLength);
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA data = {};
        data.pSysMem = m_vertexData;
        device->CreateBuffer(&desc, &data, &m_vertexBuffer);
    }

    std::free(m_vertexData);
    m_vertexData = NULL;
}

std::uint64_t Renderer::CommandBuffer::GetAllocated()
{
    return m_allocated;
}

bool Renderer::CommandBuffer::IsBusy()
{
    return false;
}

void Renderer::CommandBuffer::Render(C4JRender::eVertexType vType, Renderer::Context &c, int primitiveType)
{
    PROFILER_SCOPE("Renderer::CommandBuffer::Render", "Render", MP_ORANGE);

    if (!m_vertexBuffer)
        return;

    int drawVertexType = vType;
    int shaderVertexType = drawVertexType;
    bool matrixOverride = false;

    for (const Command &command : m_commands)
    {
        PROFILER_SCOPE("Renderer::CommandBuffer::Render", "ProcessCommand", MP_ORANGE);

        switch (command.m_command_type)
        {
        case COMMAND_ADD_MATRIX:
        {
            if (drawVertexType == C4JRender::VERTEX_TYPE_COMPRESSED)
            {
                const float row[4] = {
                    command.add_matrix.m_matrix[12], command.add_matrix.m_matrix[13],
                    command.add_matrix.m_matrix[14], command.add_matrix.m_matrix[15]
                };
                D3D11_MAPPED_SUBRESOURCE mappedAux0 = {};
                c.m_pDeviceContext->Map(c.m_compressedTranslationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedAux0);
                std::memcpy(mappedAux0.pData, row, sizeof(row));
                c.m_pDeviceContext->Unmap(c.m_compressedTranslationBuffer, 0);
            }
            else
            {
                D3D11_MAPPED_SUBRESOURCE mappedMatrix1 = {};
                c.m_pDeviceContext->Map(c.m_localTransformMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedMatrix1);
                std::memcpy(mappedMatrix1.pData, command.add_matrix.m_matrix, sizeof(command.add_matrix.m_matrix));
                c.m_pDeviceContext->Unmap(c.m_localTransformMatrix, 0);
                matrixOverride = true;
            }
            break;
        }
        case COMMAND_ADD_VERTICES:
        {
            if (isActive)
            {
                InternalRenderManager.UpdateLightingState();

                if (drawVertexType == C4JRender::VERTEX_TYPE_PF3_TF2_CB4_NB4_XW1)
                {
                    if (c.lightingEnabled)
                    {
                        drawVertexType = C4JRender::VERTEX_TYPE_PF3_TF2_CB4_NB4_XW1_LIT;
                        shaderVertexType = C4JRender::VERTEX_TYPE_PF3_TF2_CB4_NB4_XW1_LIT;
                    }
                }
                else if (drawVertexType == C4JRender::VERTEX_TYPE_PF3_TF2_CB4_NB4_XW1_LIT && !c.lightingEnabled)
                {
                    drawVertexType = C4JRender::VERTEX_TYPE_PF3_TF2_CB4_NB4_XW1;
                    shaderVertexType = C4JRender::VERTEX_TYPE_PF3_TF2_CB4_NB4_XW1;
                }

                if (static_cast<DWORD>(drawVertexType) != InternalRenderManager.activeVertexType)
                {
                    c.m_pDeviceContext->VSSetShader(InternalRenderManager.vertexShaderTable[shaderVertexType], NULL, 0);
                    c.m_pDeviceContext->IASetInputLayout(InternalRenderManager.inputLayoutTable[shaderVertexType]);
                    InternalRenderManager.activeVertexType = drawVertexType;
                }
            }

            unsigned int drawCount = command.add_vertices.m_vertex_count;
            bool drawIndexed = false;
            if (primitiveType == C4JRender::PRIMITIVE_TYPE_QUAD_LIST)
            {
                drawCount = (drawCount * 6) / 4;
                drawIndexed = true;
            }
            else if (primitiveType == C4JRender::PRIMITIVE_TYPE_TRIANGLE_FAN)
            {
                drawCount = (drawCount - 2) * 3;
                drawIndexed = true;
            }

            ID3D11Buffer *buffer = m_vertexBuffer;
            const UINT stride = InternalRenderManager.vertexStrideTable[drawVertexType];
            const UINT offset = command.add_vertices.m_vertex_index_start;
            c.m_pDeviceContext->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);

            if (drawIndexed)
                c.m_pDeviceContext->DrawIndexed(drawCount, 0, 0);
            else
                c.m_pDeviceContext->Draw(drawCount, 0);
            break;
        }
        case COMMAND_BIND_TEXTURE:
        {
            c.textureIdx = command.bind_texture.m_texture_index;
            ID3D11ShaderResourceView *view = InternalRenderManager.m_textures[c.textureIdx].view;
            c.m_pDeviceContext->PSSetShaderResources(0, 1, &view);
            InternalRenderManager.UpdateTextureState(false);
            break;
        }
        case COMMAND_SET_COLOR:
        {
            D3D11_MAPPED_SUBRESOURCE mappedColour = {};
            c.m_pDeviceContext->Map(c.m_tintColorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedColour);
            std::memcpy(mappedColour.pData, command.set_color.m_color, sizeof(command.set_color.m_color));
            c.m_pDeviceContext->Unmap(c.m_tintColorBuffer, 0);
            break;
        }
        case COMMAND_SET_DEPTH_FUNC:
        {
            c.depthStencilDesc.DepthFunc = static_cast<D3D11_COMPARISON_FUNC>(command.set_depth_func.m_depth_func);
            c.m_pDeviceContext->OMSetDepthStencilState(InternalRenderManager.GetManagedDepthStencilState(), 0);
            break;
        }
        case COMMAND_SET_DEPTH_MASK:
        {
            c.depthWriteEnabled = command.set_depth_mask.m_enable;
            c.depthStencilDesc.DepthWriteMask = command.set_depth_mask.m_enable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
            c.m_pDeviceContext->OMSetDepthStencilState(InternalRenderManager.GetManagedDepthStencilState(), 0);
            break;
        }
        case COMMAND_SET_DEPTH_TEST:
        {
            c.depthTestEnabled = command.set_depth_test.m_enable;
            c.depthStencilDesc.DepthEnable = command.set_depth_test.m_enable;
            c.m_pDeviceContext->OMSetDepthStencilState(InternalRenderManager.GetManagedDepthStencilState(), 0);
            break;
        }
        case COMMAND_SET_LIGHTING_ENABLE:
        {
            c.lightingEnabled = command.set_lighting_enable.m_enable;
            break;
        }
        case COMMAND_SET_LIGHT_ENABLE:
        {
            const int light = command.set_light_enable.m_light_index;
            if (light >= 0 && light < 2)
            {
                c.lightEnabled[light] = command.set_light_enable.m_enable;
                c.lightingDirty = true;
            }
            break;
        }
        case COMMAND_SET_LIGHT_DIRECTION:
        {
            const int light = command.set_light_direction.m_light_index;
            if (light >= 0 && light < 2)
            {
                c.lightDirection[light].x = command.set_light_direction.m_direction[0];
                c.lightDirection[light].y = command.set_light_direction.m_direction[1];
                c.lightDirection[light].z = command.set_light_direction.m_direction[2];
                c.lightDirection[light].w = command.set_light_direction.m_direction[3];
                c.lightingDirty = true;
            }
            break;
        }
        case COMMAND_SET_LIGHT_COLOUR:
        {
            const int light = command.set_light_colour.m_light_index;
            if (light >= 0 && light < 2)
            {
                c.lightColour[light].x = command.set_light_colour.m_color[0];
                c.lightColour[light].y = command.set_light_colour.m_color[1];
                c.lightColour[light].z = command.set_light_colour.m_color[2];
                c.lightColour[light].w = 1.0f;
                c.lightingDirty = true;
            }
            break;
        }
        case COMMAND_SET_LIGHT_AMBIENT_COLOUR:
        {
            c.lightAmbientColour.x = command.set_light_ambient_colour.m_color[0];
            c.lightAmbientColour.y = command.set_light_ambient_colour.m_color[1];
            c.lightAmbientColour.z = command.set_light_ambient_colour.m_color[2];
            c.lightAmbientColour.w = 1.0f;
            c.lightingDirty = true;
            break;
        }
        case COMMAND_SET_BLEND_ENABLE:
        {
            c.blendDesc.RenderTarget[0].BlendEnable = command.set_blend_enable.m_enable;
            break;
        }
        case COMMAND_SET_BLEND_FUNC:
        {
            c.blendDesc.RenderTarget[0].SrcBlend = static_cast<D3D11_BLEND>(command.set_blend_func.m_src);
            c.blendDesc.RenderTarget[0].DestBlend = static_cast<D3D11_BLEND>(command.set_blend_func.m_dst);
            c.m_pDeviceContext->OMSetBlendState(InternalRenderManager.GetManagedBlendState(), c.blendFactor, 0xFFFFFFFF);
            break;
        }
        case COMMAND_SET_BLEND_FACTOR:
        {
            const unsigned int factor = command.set_blend_factor.m_blend_factor;
            c.blendFactor[0] = float((factor >> 0) & 0xFF) / 255.0f;
            c.blendFactor[1] = float((factor >> 8) & 0xFF) / 255.0f;
            c.blendFactor[2] = float((factor >> 16) & 0xFF) / 255.0f;
            c.blendFactor[3] = float((factor >> 24) & 0xFF) / 255.0f;
            c.m_pDeviceContext->OMSetBlendState(InternalRenderManager.GetManagedBlendState(), c.blendFactor, 0xFFFFFFFF);
            break;
        }
        case COMMAND_SET_FACE_CULL:
        {
            c.rasterizerDesc.CullMode = command.set_face_cull.m_enable ? D3D11_CULL_BACK : D3D11_CULL_NONE;
            c.m_pDeviceContext->RSSetState(InternalRenderManager.GetManagedRasterizerState());
            c.faceCullEnabled = command.set_face_cull.m_enable;
            break;
        }
        default:
            break;
        }
    }

    if (matrixOverride)
    {
        const DirectX::XMMATRIX identity = DirectX::XMMatrixIdentity();
        D3D11_MAPPED_SUBRESOURCE mappedIdentity = {};
        c.m_pDeviceContext->Map(c.m_localTransformMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedIdentity);
        std::memcpy(mappedIdentity.pData, &identity, sizeof(identity));
        c.m_pDeviceContext->Unmap(c.m_localTransformMatrix, 0);
    }
}

void Renderer::CommandBuffer::SetBlendEnable(bool enable)
{
    Command command = {};
    command.m_command_type = COMMAND_SET_BLEND_ENABLE;
    command.set_blend_enable.m_enable = enable;
    m_commands.push_back(command);
}

void Renderer::CommandBuffer::SetBlendFactor(unsigned int factor)
{
    Command command = {};
    command.m_command_type = COMMAND_SET_BLEND_FACTOR;
    command.set_blend_factor.m_blend_factor = factor;
    m_commands.push_back(command);
}

void Renderer::CommandBuffer::SetBlendFunc(int src, int dst)
{
    Command command = {};
    command.m_command_type = COMMAND_SET_BLEND_FUNC;
    command.set_blend_func.m_src = src;
    command.set_blend_func.m_dst = dst;
    m_commands.push_back(command);
}

void Renderer::CommandBuffer::SetColor(float r, float g, float b, float a)
{
    Command command = {};
    command.m_command_type = COMMAND_SET_COLOR;
    command.set_color.m_color[0] = r;
    command.set_color.m_color[1] = g;
    command.set_color.m_color[2] = b;
    command.set_color.m_color[3] = a;
    m_commands.push_back(command);
}

void Renderer::CommandBuffer::SetDepthFunc(int func)
{
    Command command = {};
    command.m_command_type = COMMAND_SET_DEPTH_FUNC;
    command.set_depth_func.m_depth_func = func;
    m_commands.push_back(command);
}

void Renderer::CommandBuffer::SetDepthMask(bool enable)
{
    Command command = {};
    command.m_command_type = COMMAND_SET_DEPTH_MASK;
    command.set_depth_mask.m_enable = enable;
    m_commands.push_back(command);
}

void Renderer::CommandBuffer::SetDepthTestEnable(bool enable)
{
    Command command = {};
    command.m_command_type = COMMAND_SET_DEPTH_TEST;
    command.set_depth_test.m_enable = enable;
    m_commands.push_back(command);
}

void Renderer::CommandBuffer::SetFaceCull(bool enable)
{
    Command command = {};
    command.m_command_type = COMMAND_SET_FACE_CULL;
    command.set_face_cull.m_enable = enable;
    m_commands.push_back(command);
}

void Renderer::CommandBuffer::SetLightAmbientColour(float r, float g, float b)
{
    Command command = {};
    command.m_command_type = COMMAND_SET_LIGHT_AMBIENT_COLOUR;
    command.set_light_ambient_colour.m_color[0] = r;
    command.set_light_ambient_colour.m_color[1] = g;
    command.set_light_ambient_colour.m_color[2] = b;
    m_commands.push_back(command);
}

void Renderer::CommandBuffer::SetLightColour(int light, float r, float g, float b)
{
    Command command = {};
    command.m_command_type = COMMAND_SET_LIGHT_COLOUR;
    command.set_light_colour.m_light_index = light;
    command.set_light_colour.m_color[0] = r;
    command.set_light_colour.m_color[1] = g;
    command.set_light_colour.m_color[2] = b;
    m_commands.push_back(command);
}

void Renderer::CommandBuffer::SetLightDirection(int light, float x, float y, float z)
{
    Renderer::Context &c = InternalRenderManager.getContext();
    const std::uint32_t depth = c.stackPos[MATRIX_MODE_MODELVIEW_CBUFF];
    const DirectX::XMMATRIX &matrix = c.matrixStacks[MATRIX_MODE_MODELVIEW_CBUFF][depth];

    DirectX::XMVECTOR direction = DirectX::XMVectorSet(x, y, z, 0.0f);
    direction = DirectX::XMVector3TransformNormal(direction, matrix);
    direction = DirectX::XMVector3Normalize(direction);

    Command command = {};
    command.m_command_type = COMMAND_SET_LIGHT_DIRECTION;
    command.set_light_direction.m_light_index = light;
    DirectX::XMFLOAT4 outDirection;
    DirectX::XMStoreFloat4(&outDirection, direction);
    command.set_light_direction.m_direction[0] = outDirection.x;
    command.set_light_direction.m_direction[1] = outDirection.y;
    command.set_light_direction.m_direction[2] = outDirection.z;
    command.set_light_direction.m_direction[3] = outDirection.w;
    m_commands.push_back(command);
}

void Renderer::CommandBuffer::SetLightEnable(int light, bool enable)
{
    Command command = {};
    command.m_command_type = COMMAND_SET_LIGHT_ENABLE;
    command.set_light_enable.m_light_index = light;
    command.set_light_enable.m_enable = enable;
    m_commands.push_back(command);
}

void Renderer::CommandBuffer::SetLightingEnable(bool enable)
{
    Command command = {};
    command.m_command_type = COMMAND_SET_LIGHTING_ENABLE;
    command.set_lighting_enable.m_enable = enable;
    m_commands.push_back(command);
}

void Renderer::CommandBuffer::StartRecording() {}
