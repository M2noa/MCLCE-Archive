#include "ShaderConstants.fxh"
#include "util.fxh"

#define FAR_PLANE_DEPTH 1.0f
#define GAUSSIAN_DOF

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
#ifdef GAUSSIAN_DOF

	const float centerDepth = MakeDepthLinear(TEXTURE_1.Sample(TextureSampler1, float2(0.5f, 0.5f)).r, 0.025f, FAR_CHUNKS_DISTANCE, false);
	const float depth = MakeDepthLinear(TEXTURE_1.Sample(TextureSampler1, PSInput.uv).r, 0.025f, FAR_CHUNKS_DISTANCE, false);
	float4 color = TEXTURE_0.Sample(TextureSampler0, PSInput.uv);

	if ((centerDepth > DepthOfFieldNearEndDepth || depth >= DepthOfFieldFarStartDepth) && depth > 0.01f) {

		const float nearBlurPlaneEnd = clamp(DepthOfFieldNearEndDepth, 0, DepthOfFieldFarStartDepth);	//Can only go up to where the far plane starts
		const float farPlaneStart = clamp(DepthOfFieldFarStartDepth, DepthOfFieldNearEndDepth, DepthOfFieldFarEndDepth);
		const float farPlaneEnd = clamp(DepthOfFieldFarEndDepth, DepthOfFieldFarStartDepth, FAR_PLANE_DEPTH);
		float4 baseColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
		float4 blurColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

		if (depth >= farPlaneEnd) {	//We are beyound max blur ... so always pick the blurry one
			color = TEXTURE_3.Sample(TextureSampler3, PSInput.uv);	//Far blur
																	//color = float4(1.0f, 0.0f, 0.0f, 1.0f);
		}
		else if (depth < nearBlurPlaneEnd) {
			float lerpRatio = depth / nearBlurPlaneEnd;
			blurColor = TEXTURE_2.Sample(TextureSampler2, PSInput.uv);	//Near blur
			baseColor = TEXTURE_0.Sample(TextureSampler0, PSInput.uv);	//Clear color
																		//blurColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
																		//baseColor = float4(0.0f, 1.0f, 0.0f, 1.0f);
			color = lerp(blurColor, baseColor, lerpRatio);
		}
		else if (depth > farPlaneStart) {
			float lerpRatio = (depth - farPlaneStart) / (farPlaneEnd - farPlaneStart);
			blurColor = TEXTURE_3.Sample(TextureSampler3, PSInput.uv);	//Far blur
			baseColor = TEXTURE_0.Sample(TextureSampler0, PSInput.uv);	//Clear color
																		//blurColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
																		//baseColor = float4(0.0f, 1.0f, 0.0f, 1.0f);
			color = lerp(baseColor, blurColor, lerpRatio);
		}

	}

	PSOutput.color = color;

#else

	//Default to no depth of field
	PSOutput.color = TEXTURE_0.Sample(TextureSampler0, PSInput.uv);

#endif

}