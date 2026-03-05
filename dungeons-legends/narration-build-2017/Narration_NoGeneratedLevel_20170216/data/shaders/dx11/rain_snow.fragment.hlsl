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

	// clamp the uvs
	if (uv.x >= 0.0f && uv.x <= 1.0f && 
		uv.y >= 0.0f && uv.y <= 1.0f && 
		PSInput.worldPosition.y < occlusionTexture.a) {
		PSOutput.color.a = 0.0f;
	}

	float mixAmount = saturate((PSInput.worldPosition.y - occlusionTexture.a)*10.0f);
	float3 lighting = lerp(occlusionTexture.rgb, PSInput.color.rgb, mixAmount);
	PSOutput.color.rgb *= lighting.rgb;

	//apply fog
	PSOutput.color.rgb = lerp(PSOutput.color.rgb, PSInput.fogColor.rgb, PSInput.fogColor.a);
}


