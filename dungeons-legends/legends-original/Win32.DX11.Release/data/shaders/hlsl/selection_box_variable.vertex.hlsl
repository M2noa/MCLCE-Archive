#include "ShaderConstants.fxh"
#include "util.fxh"

struct VS_Input
{
	float3 position : POSITION;
	float4 weights : WEIGHTS;
#ifdef INSTANCEDSTEREO
	uint instanceID : SV_InstanceID;
#endif
};


struct PS_Input
{
	float4 position : SV_Position;
#ifdef GEOMETRY_INSTANCEDSTEREO
	uint instanceID : SV_InstanceID;
#endif
#ifdef VERTEXSHADER_INSTANCEDSTEREO
	uint renTarget_id : SV_RenderTargetArrayIndex;
#endif
};


void main( in VS_Input VSInput, out PS_Input PSInput )
{
#ifdef INSTANCEDSTEREO
	int i = VSInput.instanceID;
	PSInput.position = mul( WORLDVIEWPROJ_STEREO[i], float4( VSInput.position, 1 ) );
	PSInput.position.xy += computeScreenOffsets(VSInput.position.xyz, VSInput.weights, WORLDVIEWPROJ_STEREO[i]);
#ifdef GEOMETRY_INSTANCEDSTEREO
    PSInput.instanceID = i;
#endif 
#ifdef VERTEXSHADER_INSTANCEDSTEREO
	PSInput.renTarget_id = i;
#endif    
#else
	PSInput.position = mul(WORLDVIEWPROJ, float4(VSInput.position, 1));
	PSInput.position.xy += computeScreenOffsets(VSInput.position.xyz, VSInput.weights, WORLDVIEWPROJ);
#endif

    //this is to make it come closer to the camera and avoid z-fighting
    PSInput.position.z -= 0.0001;
}
