#include "ShaderConstants.fxh"

struct PS_Input
{
	float4 position : SV_Position;
	float2 uv : TEXCOORD_0_FB_MSAA;
};

struct PS_Output
{
	float depth : SV_Depth;
#if (VERSION <= 0xa000 /*D3D_FEATURE_LEVEL_10_0*/)
	float4 dx9dummy : SV_Target;
#endif
};

void main( in PS_Input PSInput, out PS_Output PSOutput )
{
#ifdef MSAA_FRAMEBUFFER_ENABLED
	float depth = 0.0f;
    int sampleCount = MSAA_SAMPLECOUNT;
	for (int i = 0; i < sampleCount; ++i )
	{
		depth += TEXTURE_0_MS.Load(PSInput.position.xy, i);
	}
	depth /= sampleCount;

	PSOutput.depth = depth;  
#else
    PSOutput.depth = 0.0;
#endif

#if (VERSION <= 0xa000 /*D3D_FEATURE_LEVEL_10_0*/)
	PSOutput.dx9dummy = float4(0, 0, 0, 0);
#endif
}