#include "ShaderConstants.fxh"

struct VS_Input
{
    float3 position : POSITION;
    float2 uv : TEXCOORD_0;
#ifdef INSTANCEDSTEREO
	uint instanceID : SV_InstanceID;
#endif
};


struct PS_Input
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD_0;
#ifdef INSTANCEDSTEREO
	uint instanceID : SV_InstanceID;
#endif
};


void main( in VS_Input VSInput, out PS_Input PSInput )
{
    PSInput.uv = VSInput.uv;
#ifdef INSTANCEDSTEREO
	int i = VSInput.instanceID;
	float4 pos = mul(WORLDVIEW_STEREO[i], float4(VSInput.position, 1));
	PSInput.position = mul(PROJ_STEREO[i], pos);
	PSInput.instanceID = i;
#else
	float4 pos = mul(WORLDVIEW, float4(VSInput.position, 1));
	PSInput.position = mul(PROJ, pos);
#endif
}