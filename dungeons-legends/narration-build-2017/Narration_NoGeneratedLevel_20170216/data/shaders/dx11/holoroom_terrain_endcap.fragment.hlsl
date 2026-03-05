//$ANTCOLONY

#include "ShaderConstants.fxh"

struct PS_Input
{
	float4 position : SV_Position;
	float4	color			: COLOR;
	float2 uv0 : TEXCOORD_0;
	float4 uv1 : TEXCOORD_1;
};

struct PS_Output
{
	float4 color : SV_Target;
};

void main(in PS_Input PSInput, out PS_Output PSOutput)
{
	float2 uv = PSInput.uv1.xy + (PSInput.uv0 - floor(PSInput.uv0)) * PSInput.uv1.zw;
	float4 diffuse = TEXTURE_0.Sample(TextureSampler0, uv);
	float2 lightingUVs = float2(0.0f, 0.9f);
	diffuse.rgb *= TEXTURE_1.Sample(TextureSampler1, lightingUVs).rgb;

	if (diffuse.a < 0.5f)
	{
		discard;
	}

	diffuse *= PSInput.color;
	PSOutput.color = diffuse;
}