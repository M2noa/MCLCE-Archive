#include "ShaderConstants.fxh"

struct VS_Input
{
	float3 position : POSITION;
	float4 normal : NORMAL;
	float4 color : COLOR;
#ifdef INSTANCEDSTEREO
	uint instanceID : SV_InstanceID;
#endif
};


struct PS_Input
{
	float4 position : SV_Position;
	float4 color : COLOR;
#ifdef INSTANCEDSTEREO
	uint instanceID : SV_InstanceID;
#endif
};


void main( in VS_Input VSInput, out PS_Input PSInput )
{
	PSInput.color = VSInput.color * 0.5f + -0.5f * dot(VSInput.normal, float4(GAZE_DIRECTION, 0.0f));
#ifdef INSTANCEDSTEREO
	int i = VSInput.instanceID;
	PSInput.position = mul( WORLDVIEWPROJ_STEREO[i], float4( VSInput.position, 1.0f ) );
	PSInput.instanceID = i;
#else
	PSInput.position = mul( WORLDVIEWPROJ, float4( VSInput.position, 1.0f ) );
#endif
}