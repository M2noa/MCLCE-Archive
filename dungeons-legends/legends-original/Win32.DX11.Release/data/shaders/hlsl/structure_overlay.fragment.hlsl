#include "ShaderConstants.fxh"
#include "util.fxh"

struct PS_Input
{
	float4 position : SV_Position;
	float2 uv : TEXCOORD_0;
};

struct PS_Output
{
	float4 color : SV_Target;

#ifdef FORCE_DEPTH_ZERO
	float depth : SV_Depth;
#endif

};

void main( in PS_Input PSInput, out PS_Output PSOutput )
{
	float4 color = CURRENT_COLOR;
	PSOutput.color = color;

#ifdef FORCE_DEPTH_ZERO
	PSOutput.depth = 0.0;
#endif
}
