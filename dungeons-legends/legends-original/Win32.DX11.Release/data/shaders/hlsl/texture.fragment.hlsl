#include "ShaderConstants.fxh"

struct PS_Input
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD_0_FB_MSAA;
};

struct PS_Output
{
    float4 color : SV_Target;
};

void main(in PS_Input PSInput, out PS_Output PSOutput)
{
#ifdef STEREO_LEFT_EYE_ONLY
    PSOutput.color = TEXTURE_0.Sample(TextureSampler0, float3(PSInput.uv, 0.0f));
#elif defined(DEPTH_TEXTURE)
	PSOutput.color.a = 1.0;
#if defined(ARRAY_TEXTURE_0)
	PSOutput.color.rgb = TEXTURE_0.Sample(TextureSampler0, float3(PSInput.uv, TEXTURE_ARRAY_INDEX_0)).r;
#else
    PSOutput.color.rgb = TEXTURE_0.Sample(TextureSampler0, PSInput.uv).r;
#endif
#else
#if defined(ARRAY_TEXTURE_0)
	PSOutput.color = TEXTURE_0.Sample(TextureSampler0, float3(PSInput.uv, TEXTURE_ARRAY_INDEX_0));
#else
    PSOutput.color = TEXTURE_0.Sample(TextureSampler0, PSInput.uv);
#endif
#endif

#ifdef ALPHA_TEST
    if( PSOutput.color.a < 0.5 )
    {
        discard;
    }
#endif

#ifdef NO_ALPHA
	PSOutput.color.a = 1.f;
#endif
}