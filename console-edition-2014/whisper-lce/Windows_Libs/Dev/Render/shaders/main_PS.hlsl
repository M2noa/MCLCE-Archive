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
cbuffer diffuse : register(b0)
{
    float4 diffuse_colour;
};
cbuffer fog : register(b1)
{
    float4 fog_colour;
};
cbuffer alphaTest : register(b3)
{
    float4 alphaTestRef;
};

#ifdef FORCE_LOD
cbuffer forcedLOD : register(b5)
{
    float4 forcedLod;
};
#endif

SamplerState diffuse_sampler_s : register(s0);
Texture2D<float4> diffuse_texture : register(t0);

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 colour : COLOR0;
    linear centroid float4 texcoord : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float2 uv = input.texcoord.xy;

    #ifdef TEXTURE_PROJECTION
    uv /= input.texcoord.w;
    #endif

    #ifdef FORCE_LOD
    float4 texel = diffuse_texture.SampleLevel(
        diffuse_sampler_s,
        uv,
        forcedLod.x);
    #else
    float4 texel =
        (uv.x > 1.0f)
        ? diffuse_texture.SampleLevel(diffuse_sampler_s, uv, 0)
        : diffuse_texture.Sample(diffuse_sampler_s, uv);
    #endif

    float4 colour = texel * diffuse_colour;
    colour.a *= input.colour.a;

    if (colour.a < alphaTestRef.w)
        discard;

    float3 shaded = colour.rgb * input.colour.rgb;
    float3 fogged = lerp(fog_colour.rgb, shaded, input.texcoord.z);

    return float4(fogged, colour.a);
}