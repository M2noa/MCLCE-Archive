#include "ShaderConstants.fxh"

struct PS_Input
{
	float4 position : SV_Position;
	float4 color : COLOR;
	float2 uv : TEXCOORD_0;
};

struct PS_Output
{
	float4 color : SV_Target;
};

void main( in PS_Input PSInput, out PS_Output PSOutput )
{
	float2 uv = PSInput.uv * SKIRT_UV_SCALE.xy;
	uv = frac(uv);
	uv = uv * SKIRT_UVS.zw + SKIRT_UVS.xy;
	PSOutput.color = TEXTURE_0.Sample( TextureSampler0, uv ) * PSInput.color.r;
	PSOutput.color.rgb *= TEXTURE_1.Sample(TextureSampler1, PSInput.color.gr).rgb;
}