#include "ShaderConstants.fxh"

struct PS_Input
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD_0;
};

struct PS_Output
{
    float4 color : SV_Target;
};

void main( in PS_Input PSInput, out PS_Output PSOutput )
{
	// when copying the levelstage texture for the rift overlay, we need to ensure alpha is 1, and
	// convert from linear space to SRGB

    PSOutput.color = TEXTURE_0.Sample( TextureSampler0, PSInput.uv );
	PSOutput.color.a = 1.0;
	PSOutput.color = pow(PSOutput.color, 2.2); // conversion to srgb

}