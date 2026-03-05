#include "ShaderConstants.fxh"

struct PS_Input
{
    float4 position : SV_Position;
    float4 color : COLOR;
    float2 uv : TEXCOORD_0_FB_MSAA;
};

struct PS_Output
{
    float4 color : SV_Target;
};

float median(float a, float b, float c) {
    return max(min(a, b), min(max(a, b), c));
}

static const float GLYPH_UV_SIZE = 1.0 / 16.0;

void main(in PS_Input PSInput, out PS_Output PSOutput)
{
    float4 diffuse = TEXTURE_0.Sample(TextureSampler0, PSInput.uv);

#ifdef SMOOTH
    const float center = 0.4f;
    const float radius = 0.1f;

    diffuse = smoothstep(center - radius, center + radius, diffuse);
#endif

#ifdef ALPHA_TEST
    if (diffuse.a < 0.5)
    {
        discard;
    }
#endif

#ifdef MSDF
    float4 resultColor = PSInput.color;

    const float sampleDistance = median(diffuse.r, diffuse.g, diffuse.b);

    const float innerEdgeAlpha = smoothstep(max(0.0, GLYPH_CUTOFF - GLYPH_SMOOTH_RADIUS), min(1.0, GLYPH_CUTOFF + GLYPH_SMOOTH_RADIUS), sampleDistance);
    // Apply fill cutoff
    resultColor = lerp(OUTLINE_COLOR, resultColor, innerEdgeAlpha);

    const float outerEdgeAlpha = smoothstep(max(0.0, OUTLINE_CUTOFF - GLYPH_SMOOTH_RADIUS), min(1.0, OUTLINE_CUTOFF + GLYPH_SMOOTH_RADIUS), sampleDistance);
    // Apply stroke (outline) cutoff
    resultColor = float4(resultColor.rgb, resultColor.a * outerEdgeAlpha);

    const float2 topLeft = floor(PSInput.uv / GLYPH_UV_SIZE) * GLYPH_UV_SIZE;
    const float2 bottomRight = floor(PSInput.uv / GLYPH_UV_SIZE) * GLYPH_UV_SIZE + GLYPH_UV_SIZE;

    const float4 shadowSample = TEXTURE_0.Sample(TextureSampler0, clamp(PSInput.uv - SHADOW_OFFSET, topLeft, bottomRight));
    const float shadowDistance = shadowSample.a;
    const float shadowAlpha = smoothstep(max(0.0, OUTLINE_CUTOFF - SHADOW_SMOOTH_RADIUS), min(1.0, OUTLINE_CUTOFF + SHADOW_SMOOTH_RADIUS), shadowDistance);
    // Apply shadow past the stroke
    resultColor = lerp(float4(SHADOW_COLOR.rgb, SHADOW_COLOR.a * shadowAlpha), resultColor, outerEdgeAlpha);

    diffuse = resultColor;
#else
    diffuse *= PSInput.color;
#endif

    PSOutput.color = diffuse * DARKEN;
}