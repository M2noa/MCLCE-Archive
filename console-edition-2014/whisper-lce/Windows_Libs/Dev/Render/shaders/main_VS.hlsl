
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
#ifndef COMPRESSED

cbuffer positionTransformWV : register(b0)
{
    float4x4 matWorldView;
};
cbuffer positionTransformWV2 : register(b1)
{
    float4x4 matWorldView2;
};
cbuffer positionTransformProj : register(b2)
{
    float4x4 matProjection;
};
cbuffer textureTransform : register(b3)
{
    float4x4 matUV;
};
cbuffer textureUV2 : register(b4)
{
    float4 vecUVT2;
};
cbuffer fog : register(b5)
{
    float4 vecFog;
};

#ifdef LIGHTING
cbuffer lighting : register(b6)
{
    float3 vecLight0;
    float3 vecLight1;
    float4 vecLight0Col;
    float4 vecLight1Col;
    float4 vecLightAmbientCol;
};
#endif

#ifdef TEXGEN
cbuffer texgen : register(b7)
{
    float4x4 matTexGenView;
    float4x4 matTexGenObj;
};
#endif

#else

cbuffer positionTransformWV : register(b0)
{
    float4x4 matWorldView;
};
cbuffer positionTransformProj : register(b2)
{
    float4x4 matProjection;
};
cbuffer textureUV2 : register(b4)
{
    float4 vecUVT2;
};
cbuffer fog : register(b5)
{
    float4 vecFog;
};
cbuffer positionTransform2 : register(b8)
{
    float4 vecWV2Trans;
};

#endif

SamplerState light_sampler_s : register(s0);
Texture2D<float4> light_texture : register(t0);

#ifndef COMPRESSED
struct VS_INPUT
{
    float4 position : POSITION0;
    float4 colour : COLOR0;
    float3 normal : NORMAL0;
    float4 texCoord : TEXCOORD0;
    int2 lightMapCoord : TEXCOORD1;
};
#else
struct VS_INPUT
{
    int4 position : POSITION0;
    int4 texCoord : TEXCOORD0;
};
#endif

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 colour : COLOR0;
    float4 texCoord : TEXCOORD0;
};

float CalcFogFactor(float4 vecFog, float viewZ)
{
    float fogLinear = saturate(vecFog.y * (vecFog.x + viewZ));
    float fogExp = min(1.0f, exp2(1.44269502f * vecFog.x * viewZ));
    float f = (vecFog.z == 2) ? fogExp : 1.0f;
    return (vecFog.z == 1) ? fogLinear : f;

}

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    #ifndef COMPRESSED
    
    float4 skinnedPos = mul(matWorldView2, input.position);
    float4 cameraPos = mul(matWorldView, skinnedPos);
    output.position = mul(matProjection, cameraPos);

    float2 lightUV = frac(max(vecUVT2.xy, float2(input.lightMapCoord) * 0.00390625f));
    float4 lightSample = light_texture.SampleLevel(light_sampler_s, lightUV, 0);
    lightSample.w = 1.0f;
    
    #ifdef LIGHTING
    float3 skinNormal = mul((float3x3)matWorldView2, input.normal);
    float3 viewNormal = normalize(mul((float3x3)matWorldView, skinNormal));

    float diffuse0 = max(0.0f, dot(vecLight0, viewNormal));
    float diffuse1 = max(0.0f, dot(vecLight1, viewNormal));
    float4 litColour = saturate(vecLightAmbientCol + diffuse0 * vecLight0Col + diffuse1 * vecLight1Col);

    output.colour.xyz = litColour.xyz * (input.colour.wzy * lightSample.xyz);
    output.colour.w = litColour.w;
    #else
    output.colour = input.colour.wzyx * lightSample;
    #endif

    #ifdef TEXGEN
    float4 texGenCoords = mul(matTexGenView, cameraPos) + mul(matTexGenObj, input.position);
    output.texCoord.xy = float2(dot(matUV[0], texGenCoords), dot(matUV[1], texGenCoords));
    output.texCoord.w  = dot(matUV[3], texGenCoords);
    #else
    output.texCoord.xy = float2(dot(matUV[0], input.texCoord), dot(matUV[1], input.texCoord));
    output.texCoord.w = 1.0f;
    #endif

    output.texCoord.z = CalcFogFactor(vecFog, cameraPos.z);

    #else // COMPRESSED

    float4 pos;
    pos.xyz = float3((int3)input.position.xyz) * 0.0009765625f + vecWV2Trans.xyz;
    pos.w = 1.0f;

    int packedColor = (int)input.position.w + 32768;
    float3 color = frac(packedColor * float3(1.52587891e-05f, 0.00048828125f, 0.03125f));

    float4 cameraPos = mul(matWorldView, pos);
    output.position = mul(matProjection, cameraPos);

    float4 uvs = float4((int4)input.texCoord.zwxy) * float4(0.00390625f, 0.00390625f, 0.000122070312f, 0.000122070312f);
    float2 lightUV = frac(max(vecUVT2.xy, uvs.xy));
    float4 lightSample = light_texture.SampleLevel(light_sampler_s, lightUV, 0);

    output.colour.xyz = lightSample.xyz * color;
    output.colour.w = 1.0f;
    output.texCoord.xy = uvs.zw;
    output.texCoord.z = CalcFogFactor(vecFog, cameraPos.z);
    output.texCoord.w = 1.0f;

    #endif

    return output;
}
