#include "ShaderConstants.fxh"

struct VS_Input
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float4 color : COLOR;
	float2 texCoords : TEXCOORD_0;
#ifdef INSTANCEDSTEREO
	uint instanceID : SV_InstanceID;
#endif
};


struct PS_Input
{
	float4 position : SV_Position;
	float4 color : COLOR;
	float2 texCoords : TEXCOORD_0;
#ifdef INSTANCEDSTEREO
	uint instanceID : SV_InstanceID;
#endif
};


void main( in VS_Input VSInput, out PS_Input PSInput )
{
	PSInput.color = VSInput.color;
	float3 position = VSInput.position;
	float3 voxelCenter = VSInput.normal * 80.0f - float3(40.0f, 40.0f, 40.0f);
	voxelCenter.y = -0.5f;
	float2 delta = voxelCenter.xz - TABLE_REVEAL_CENTER.xz;
	float lengthSq = dot(delta, delta);

	if (lengthSq > (TABLE_REVEAL_CONSTANTS.x * TABLE_REVEAL_CONSTANTS.x)) {
		position = float3(0.0f, 0.0f, 0.0f);
	}
	else if (TABLE_REVEAL_CONSTANTS.y > 0.0f && VSInput.normal.y > 0.0f && lengthSq < (TABLE_REVEAL_CONSTANTS.y * TABLE_REVEAL_CONSTANTS.y)) {
		float fallAmount = TABLE_REVEAL_CONSTANTS.y*TABLE_REVEAL_CONSTANTS.y - lengthSq;
		position.y -= fallAmount * 0.1f;
	}

#ifdef INSTANCEDSTEREO
	int i = VSInput.instanceID;
	PSInput.position = mul( WORLDVIEWPROJ_STEREO[i], float4( position, 1 ) );
	PSInput.instanceID = i;
#else
	PSInput.position = mul(WORLDVIEWPROJ, float4(position, 1));
#endif
	PSInput.texCoords = VSInput.texCoords;
}