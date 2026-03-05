#include "ShaderConstants.fxh"

struct GeometryShaderInput {
	float4		pos				: SV_POSITION;
#ifdef ENABLE_LIGHT
	float4		light			: LIGHT;
#endif
#ifdef ENABLE_FOG
	float4		fogColor		: FOG_COLOR;
#endif
#ifndef DISABLE_TINTING
	float4		color			: COLOR;
#endif
	float4		uv				: TEXCOORD_0_FB_MSAA;

#ifdef INSTANCEDSTEREO
	uint		instanceID		: SV_InstanceID;
#endif
};

// Per-pixel color data passed through the pixel shader.
struct GeometryShaderOutput {
	float4		pos				: SV_POSITION;
#ifdef ENABLE_LIGHT
	float4		light			: LIGHT;
#endif
#ifdef ENABLE_FOG
	float4		fogColor		: FOG_COLOR;
#endif
#ifndef DISABLE_TINTING
	float4		color			: COLOR;
#endif
	float4		uv				: TEXCOORD_0_FB_MSAA;

#ifdef INSTANCEDSTEREO
	uint        renTarget_id	: SV_RenderTargetArrayIndex;
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
	for (int j = 0; j < 3; j++)
	{
		output.pos = input[j].pos;
		output.uv				= input[j].uv;
#ifdef INSTANCEDSTEREO
		output.renTarget_id = i;
#endif
#ifdef ENABLE_LIGHT
		output.light			= input[j].light;
#endif
#ifdef ENABLE_FOG
		output.fogColor			= input[j].fogColor;
#endif
#ifndef DISABLE_TINTING
		output.color		    = input[j].color;
#endif
		outStream.Append(output);
	}
}