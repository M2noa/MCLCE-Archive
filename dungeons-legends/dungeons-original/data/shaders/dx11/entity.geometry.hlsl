#include "ShaderConstants.fxh"

struct GeometryShaderInput
{
	float4		pos				: SV_POSITION;
	float4		light			: LIGHT;
	float4		fogColor		: FOG_COLOR;

#ifdef GLINT
	float4		layerUV			: GLINT_UVS;
#endif

#ifdef USE_OVERLAY
	float4		overlayColor	: OVERLAY_COLOR;
#endif

	float2		uv				: TEXCOORD_0_FB_MSAA;
#ifdef INSTANCEDSTEREO
	uint		instanceID		: SV_InstanceID;
#endif
};

// Per-pixel color data passed through the pixel shader.
struct GeometryShaderOutput
{
	float4		pos				: SV_POSITION;
	float4		light			: LIGHT;
	float4		fogColor		: FOG_COLOR;

#ifdef GLINT
	float4		layerUV			: GLINT_UVS;
#endif

#ifdef USE_OVERLAY
	float4		overlayColor	: OVERLAY_COLOR;
#endif

	float2		uv				: TEXCOORD_0_FB_MSAA;
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
#ifndef NO_TEXTURE
		output.uv				= input[j].uv;
#endif
#ifdef INSTANCEDSTEREO
		output.renTarget_id = i;
#endif
		output.light			= input[j].light;
		output.fogColor			= input[j].fogColor;
#ifdef USE_OVERLAY
		output.overlayColor		= input[j].overlayColor;
#endif
#ifdef GLINT
		output.layerUV			= input[j].layerUV;
#endif
		outStream.Append(output);
	}
}