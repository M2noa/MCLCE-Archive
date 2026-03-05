#include "ShaderConstants.fxh"

struct GeometryShaderInput
{
	float4		pos				: SV_POSITION;
	float4		color			: COLOR;
	float2		uv0				: TEXCOORD_0;

#ifdef ENABLE_FOG
	float4 fogColor : FOG_COLOR;
#endif

#ifdef GLINT
	float2 layer1UV : UV_1;
	float2 layer2UV : UV_2;
#endif
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

#ifdef ENABLE_FOG
	float4 fogColor : FOG_COLOR;
#endif

#ifdef GLINT
	float2 layer1UV : UV_1;
	float2 layer2UV : UV_2;
#endif
#ifdef INSTANCEDSTEREO
	uint        renTarget_id : SV_RenderTargetArrayIndex;
#endif
};

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
#ifdef ENABLE_FOG
			output.fogColor			= input[j].fogColor;
#endif

#ifdef GLINT
			output.layer1UV = input[j].layer1UV;
			output.layer2UV = input[j].layer2UV;
#endif

#ifdef INSTANCEDSTEREO
			output.renTarget_id = i;
#endif
			outStream.Append(output);
		}
	}
}