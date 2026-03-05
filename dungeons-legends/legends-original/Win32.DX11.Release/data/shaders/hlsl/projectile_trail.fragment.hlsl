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
	float a = abs(PSInput.uv.x);
	float4 clr = PSInput.color;
	clr.a *= 1.f - a;
    PSOutput.color = clr;
}
