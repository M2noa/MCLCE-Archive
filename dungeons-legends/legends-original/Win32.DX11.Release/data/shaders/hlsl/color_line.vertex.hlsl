#include "ShaderConstants.fxh"
#include "util.fxh"

struct VS_Input
{
    float3 position : POSITION;
    float4 color : COLOR;
	float4 weights : WEIGHTS;
#ifdef INSTANCEDSTEREO
	uint instanceID : SV_InstanceID;
#endif
};

struct PS_Input
{
    float4 position : SV_Position;
    float4 color : COLOR;
#ifdef GEOMETRY_INSTANCEDSTEREO
	uint instanceID : SV_InstanceID;
#endif
#ifdef VERTEXSHADER_INSTANCEDSTEREO
	uint renTarget_id : SV_RenderTargetArrayIndex;
#endif
};

void main( in VS_Input VSInput, out PS_Input PSInput )
{
	PSInput.color = VSInput.color;
#ifdef INSTANCEDSTEREO
	int i = VSInput.instanceID;
#endif

#ifdef INSTANCEDSTEREO
	PSInput.position = mul(WORLDVIEWPROJ_STEREO[i], float4(VSInput.position.xyz, 1));
	PSInput.position.xy += computeScreenOffsets(VSInput.position.xyz, VSInput.weights, WORLDVIEWPROJ_STEREO[i][i]);
#else
	PSInput.position = mul(WORLDVIEWPROJ, float4(VSInput.position.xyz, 1));
	PSInput.position.xy += computeScreenOffsets(VSInput.position.xyz, VSInput.weights, WORLDVIEWPROJ);
#endif

#ifdef GEOMETRY_INSTANCEDSTEREO
	PSInput.instanceID = i;
#endif 
#ifdef VERTEXSHADER_INSTANCEDSTEREO
	PSInput.renTarget_id = i;
#endif
}
