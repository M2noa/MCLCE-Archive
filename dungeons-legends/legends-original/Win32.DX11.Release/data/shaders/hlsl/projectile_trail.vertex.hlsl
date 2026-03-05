#include "ShaderConstants.fxh"
#include "util.fxh"

struct VS_Input
{
    float3 position : POSITION;
    float4 color : COLOR;
	float4 weights : WEIGHTS;
	float2 texCoords : TEXCOORD_0;
#ifdef INSTANCEDSTEREO
	uint instanceID : SV_InstanceID;
#endif
};

struct PS_Input
{
    float4 position : SV_Position;
    float4 color : COLOR;
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
	PSInput.color = VSInput.color;
	PSInput.uv = float2(VSInput.texCoords.x * 2.f - 1.f, 0.f);

#ifdef INSTANCEDSTEREO
	int i = VSInput.instanceID;
#endif

	float3 pos = VSInput.position.xyz;
	float3 up = normalize(VSInput.weights.xyz);
	float dir = VSInput.weights.w;

	pos += up * dir;

#ifdef INSTANCEDSTEREO
	PSInput.position = mul(WORLDVIEWPROJ_STEREO[i], float4(pos.xyz, 1));
#else
	PSInput.position = mul(WORLDVIEWPROJ, float4(pos.xyz, 1));
#endif

#ifdef GEOMETRY_INSTANCEDSTEREO
	PSInput.instanceID = i;
#endif 
#ifdef VERTEXSHADER_INSTANCEDSTEREO
	PSInput.renTarget_id = i;
#endif
}
