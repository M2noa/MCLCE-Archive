#include "ShaderConstants.fxh"

struct PS_Input
{
	float4 position : SV_Position;
	float2 uv : TEXCOORD_0;
	float2 uv1 : TEXCOORD_1;
};

struct PS_Output
{
	float4 color : SV_Target;
};

void main(in PS_Input PSInput, out PS_Output PSOutput)
{
	float4 color = TEXTURE_0.Sample(TextureSampler0, PSInput.uv);
	float4 color1 = TEXTURE_0.Sample(TextureSampler0, PSInput.uv1);

	if (color.a < 0.01f)
	{
		color = color1;
	}
	else if (color.b > 0.01f) {
		color = lerp(color, color1, CURRENT_COLOR.a);
	}

	PSOutput.color = color;
}
