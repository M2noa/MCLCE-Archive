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
	PSOutput.color = float4(0.0, 0.0, 0.0, 1.0);	//If we don't initialize this, compiler complaints :(
#if (VERSION >= 0xa100 /*D3D_FEATURE_LEVEL_10_1*/)

	float4 samples = TEXTURE_0.Gather(TextureSampler0, PSInput.uv);
	PSOutput.color.r = min(min(samples.x, samples.y), min(samples.z, samples.w));

	// Don't reflect things that are too close to the player.  IE ignore all low depth values ( the player's hands model for example ).  We do this by pushing them to the far plane in the HiZ down sampling.
	// If color.r is smaller than 0.1, force it to be 1.0
	const float NEAR_TOLERANCE = 0.1;
	PSOutput.color.r = lerp(1.0, PSOutput.color.r, sign(saturate(PSOutput.color.r - NEAR_TOLERANCE)));

#else

													//Also works, but slower than using the gather command.  Can be used for backward compatibility with older shaders.  NOTE: GLSL also has a gather equivalent.

													// Take minimum values.
	float x_off = 0.5 / TEXTURE_DIMENSIONS.x;
	float y_off = 0.5 / TEXTURE_DIMENSIONS.y;

	float2 uv = PSInput.uv;
	float depth0 = TEXTURE_0.Sample(TextureSampler0, uv + float2(-x_off, -y_off)).r;
	float depth1 = TEXTURE_0.Sample(TextureSampler0, uv + float2(x_off, -y_off)).r;
	float depth2 = TEXTURE_0.Sample(TextureSampler0, uv + float2(-x_off, y_off)).r;
	float depth3 = TEXTURE_0.Sample(TextureSampler0, uv + float2(x_off, y_off)).r;
	PSOutput.color.r = min(min(depth0, depth1), min(depth2, depth3));
#endif

}