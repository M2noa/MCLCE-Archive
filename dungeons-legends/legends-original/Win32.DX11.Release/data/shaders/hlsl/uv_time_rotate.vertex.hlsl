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
#ifdef GEOMETRY_INSTANCEDSTEREO
	uint instanceID : SV_InstanceID;
#endif
#ifdef VERTEXSHADER_INSTANCEDSTEREO
	uint renTarget_id : SV_RenderTargetArrayIndex;
#endif
};


void main( in VS_Input VSInput, out PS_Input PSInput )
{

    PSInput.uv = VSInput.uv;
#ifdef INSTANCEDSTEREO
    int i = VSInput.instanceID;
    PSInput.position = mul(WORLDVIEWPROJ_STEREO[i], mul(CUBE_MAP_ROTATION, float4( VSInput.position, 1 )));
#ifdef GEOMETRY_INSTANCEDSTEREO
    PSInput.instanceID = i;
#endif 
#ifdef VERTEXSHADER_INSTANCEDSTEREO
	PSInput.renTarget_id = i;
#endif    
#else
    PSInput.position = mul(WORLDVIEWPROJ, mul(CUBE_MAP_ROTATION, float4(VSInput.position, 1)));
#endif
}
