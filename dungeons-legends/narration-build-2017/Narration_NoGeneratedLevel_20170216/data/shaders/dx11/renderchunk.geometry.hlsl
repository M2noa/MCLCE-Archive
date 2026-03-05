#include "ShaderConstants.fxh"

struct GeometryShaderInput
{
	float4			pos				: SV_POSITION;
	lpfloat4		color				: COLOR;
	snorm float2		uv0				: TEXCOORD_0;
	snorm float2		uv1				: TEXCOORD_1;
#ifdef NEAR_WATER
	float 			cameraDist 			: TEXCOORD_2;
#endif
#ifdef FOG
	float4				fogColor		: FOG_COLOR;
#endif
#ifdef INSTANCEDSTEREO
	uint				instanceID		: SV_InstanceID;
#endif
};

// Per-pixel color data passed through the pixel shader.
struct GeometryShaderOutput
{
	float4				pos				: SV_POSITION;
	lpfloat4			color			: COLOR;
	snorm float2		uv0				: TEXCOORD_0;
	snorm float2		uv1				: TEXCOORD_1;
#ifdef NEAR_WATER
	float 			cameraDist 			: TEXCOORD_2;
#endif
#ifdef FOG
	float4				fogColor		: FOG_COLOR;
#endif
#ifdef INSTANCEDSTEREO
	uint				renTarget_id : SV_RenderTargetArrayIndex;
#endif
};

bool inBounds(float3 worldPos)
{
	bool inBounds = true;
	if (worldPos.x < CHUNK_CLIP_MIN.x ||
		worldPos.x > CHUNK_CLIP_MAX.x ||
		worldPos.z < CHUNK_CLIP_MIN.y ||
		worldPos.z > CHUNK_CLIP_MAX.y)
	{
		inBounds = false;
	}

	return inBounds;
}

// passes through the triangles, except changint the viewport id to match the instance
[maxvertexcount(3)]
void main(triangle GeometryShaderInput input[3], inout TriangleStream<GeometryShaderOutput> outStream)
{
	GeometryShaderOutput output = (GeometryShaderOutput)0;

#ifdef INSTANCEDSTEREO
	int i = input[0].instanceID;
#endif
	{
		for (int j = 0; j < 3; j++)
		{
			output.pos = input[j].pos;
			output.color			= input[j].color;
			output.uv0				= input[j].uv0;
			output.uv1				= input[j].uv1;
#ifdef NEAR_WATER
			output.cameraDist		= input[j].cameraDist;
#endif

#ifdef INSTANCEDSTEREO
			output.renTarget_id = i;
#endif

#ifdef FOG
			output.fogColor			= input[j].fogColor;
#endif
			outStream.Append(output);
		}
	}
}