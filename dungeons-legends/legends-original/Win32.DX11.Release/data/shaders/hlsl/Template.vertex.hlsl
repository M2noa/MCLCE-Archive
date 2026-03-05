#include "ShaderConstants.fxh"

struct VS_Input
{
    float4 position : POSITION;
    float4 color : COLOR;
#ifdef INSTANCEDSTEREO
	uint instanceID : SV_InstanceID;
#endif
};


struct PS_Input
{
    float4 position : SV_Position;
    float4 color : COLOR;
};


void main( in VS_Input VSInput, out PS_Input PSInput )
{
    PSInput.color = VSInput.color;
#ifdef INSTANCEDSTEREO
	int i = VSInput.instanceID;
	PSInput.position = mul( WORLDVIEWPROJ_STEREO[i], VSInput.position );
#else
	PSInput.position = mul(WORLDVIEWPROJ, VSInput.position);
#endif
}