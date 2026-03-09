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
cbuffer screenspace_constants : register(b9)
{
    float4 v_scaleoffset;
};

void VS_ScreenSpace(uint vertex_id : SV_VertexID, out float4 position : SV_POSITION, out float2 texcoord : TEXCOORD0)
{
    float2 corner = float2(
        (vertex_id << 1) & 2,
        vertex_id & 2
    );

    position = float4(corner * float2(2, -2) + float2(-1, 1), 1, 1);
    texcoord = corner * 0.5 * v_scaleoffset.zw + v_scaleoffset.xy;
}

void VS_ScreenClear(uint vertex_id : SV_VertexID, out float4 position : SV_POSITION)
{
    float2 corner = float2(
        (vertex_id << 1) & 2,
        vertex_id & 2
    );

    position = float4(corner * float2(2, -2) + float2(-1, 1), 1, 1);
}