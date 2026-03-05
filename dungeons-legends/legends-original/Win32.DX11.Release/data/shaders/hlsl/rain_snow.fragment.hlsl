#include "ShaderConstants.fxh"

struct PS_Input {
	float4 position : SV_Position;
	float2 uv : TEXCOORD_0;
	float4 color : COLOR;
	float4 worldPosition : TEXCOORD_1;
	float4 fogColor : FOG_COLOR;
};

struct PS_Output {
	float4 color : SV_Target;
};

void main( in PS_Input PSInput, out PS_Output PSOutput ) {
	
	PSOutput.color = TEXTURE_0.Sample(TextureSampler0, PSInput.uv);

	PSOutput.color.a *= PSInput.color.a;

	float2 uv = PSInput.worldPosition.xz;
	float4 occlusionTexture = TEXTURE_1.Sample(TextureSampler1, uv);

#ifndef FLIP_OCCLUSION
#define OCCLUSION_OPERATOR <
#else
#define OCCLUSION_OPERATOR >
#endif

#ifdef SNOW
#define OCCLUSION_HEIGHT occlusionTexture.g
#define OCCLUSION_LUMINANCE occlusionTexture.r
#else
#define OCCLUSION_HEIGHT occlusionTexture.a
#define OCCLUSION_LUMINANCE occlusionTexture.b
#endif

	// clamp the uvs
	if (uv.x >= 0.0f && uv.x <= 1.0f && 
		uv.y >= 0.0f && uv.y <= 1.0f && 
		PSInput.worldPosition.y OCCLUSION_OPERATOR OCCLUSION_HEIGHT) {
		PSOutput.color.a = 0.0f;
	}

	float mixAmount = (PSInput.worldPosition.y - OCCLUSION_HEIGHT)*25.0f;
	float2 lightingUVs = float2(OCCLUSION_LUMINANCE, 1.0f);
	lightingUVs.x = lerp(lightingUVs.x, 0.0f, mixAmount);

	float3 lighting = TEXTURE_2.Sample(TextureSampler2, lightingUVs);
	PSOutput.color.rgb *= lighting.rgb;

	//apply fog
	PSOutput.color.rgb = lerp(PSOutput.color.rgb, PSInput.fogColor.rgb, PSInput.fogColor.a);
}


