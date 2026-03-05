//$ANTCOLONY
#include "ShaderConstants.fxh"

struct VS_Input
{
	float3 position : POSITION;
	float2 uv0 : TEXCOORD_0;	// x,y 0-1 based off if voxel texture wasn't in atlas. Can multiply against atlas UV's to get atlas uv's
	float2 uv1 : TEXCOORD_1;	// x = voxel coord x, y = voxel coord y
#ifdef INSTANCEDSTEREO
	uint instanceID : SV_InstanceID;
#endif
};

struct PS_Input
{
	float4 position : SV_Position;
	float4	color			: COLOR;
	float2 uv0 : TEXCOORD_0;
	float4 uvRect : TEXCOORD_1;
#ifdef INSTANCEDSTEREO
	uint instanceID : SV_InstanceID;
#endif
};

Texture2D<float4> TILE_UV_MAP : register (t1);

void main( in VS_Input VSInput, out PS_Input PSInput )
{
	float3 position = VSInput.position.xyz;

	int2 tileUVs = int2(0, VSInput.uv1.x * 255.0f);
	float4 uvCoords = TEXTURE_1[tileUVs];

	tileUVs.x = 4.0f;
	float4 colorVal = TEXTURE_1[tileUVs];

	PSInput.uv0 = VSInput.uv0 * float2(1.0f, 127.0f);
	PSInput.uvRect = uvCoords;

#ifdef INSTANCEDSTEREO
	int i = VSInput.instanceID;
	PSInput.position = mul(WORLDVIEWPROJ_STEREO[i], float4(position.xyz, 1));
	PSInput.instanceID = i;
#else
	PSInput.position = mul(WORLDVIEWPROJ, float4(position.xyz, 1));
#endif
	PSInput.color = colorVal;
}