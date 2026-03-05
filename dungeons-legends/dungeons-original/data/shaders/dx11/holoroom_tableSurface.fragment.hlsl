#include "ShaderConstants.fxh"

struct PS_Input
{
	float4 position : SV_Position;
	float3 color : COLOR;
	float2 uv : TEXCOORD_0;

};

struct PS_Output
{
	float4 color : SV_Target;
};

void main( in PS_Input PSInput, out PS_Output PSOutput )
{
	float4 diffuse = TEXTURE_0.Sample( TextureSampler0, PSInput.uv );
	//diffuse.rgb *= TEXTURE_1.Sample(TextureSampler1, float2(0, PSInput.color.a)).rgb;
	//diffuse.rgb *= float3(151.0f/255.0f, 59.0f/255.0f, 224.0f/255.0f);
	diffuse.rgb *= PSInput.color.rgb;

	PSOutput.color = diffuse;
}