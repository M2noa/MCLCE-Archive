#include "ShaderConstants.fxh"

struct GeometryShaderInput
{
	float4	pos				: SV_POSITION;
	float4		color			: COLOR;
	float2		uv0				: TEXCOORD_0;
	float4		uv1				: TEXCOORD_1;
#ifdef INSTANCEDSTEREO
	uint instanceID : SV_InstanceID;
#endif
};

// Per-pixel color data passed through the pixel shader.
struct GeometryShaderOutput
{
	float4		pos				: SV_POSITION;
	float4		color			: COLOR;
	float2		uv0				: TEXCOORD_0;
	float4		uv1				: TEXCOORD_1;
#ifdef INSTANCEDSTEREO
	uint			renTarget_id	: SV_RenderTargetArrayIndex;
#endif
};

// Takes verts in world space and transforms them by two different view 
// proj matrices, on two different viewports.
[maxvertexcount(6)]
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
#ifdef INSTANCEDSTEREO
			output.renTarget_id = i;
#endif
			outStream.Append(output);
		}
		//outStream.RestartStrip();
	}
}