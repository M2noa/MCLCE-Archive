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

	const float cycleTime = 1.f;
	const float cutoff = cycleTime * 0.75f;

	float a = mod(TIME, cycleTime) / cutoff;
	a = 1.f - smoothstep(saturate(a - 0.1f), a, PSInput.uv.x);
	if (a > 0.f) {
		color = lerp(float4(1, 1, 1, 1), color, a);
		a = 1.f;
	}
	color.a = a;

	PSOutput.color = color;

#ifdef FORCE_DEPTH_ZERO
	PSOutput.depth = 0.0;
#endif
}
