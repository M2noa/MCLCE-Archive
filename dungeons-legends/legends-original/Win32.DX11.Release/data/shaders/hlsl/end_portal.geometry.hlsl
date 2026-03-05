#include "ShaderConstants.fxh"

struct GeometryShaderInput
{
	float4 pos : SV_POSITION;
	#ifndef BYPASS_PIXEL_SHADER
		snorm float2 colorLookupUV : TEXCOORD_0_FB_MSAA;
		float2 parallaxUV : TEXCOORD_1_FB_MSAA;
		float4 encodedPlane : PLANE_INFO;
		#ifdef FOG
			float4 fogColor : FOG_COLOR;
		#endif
	#endif
	#ifdef INSTANCEDSTEREO
		uint instanceID : SV_InstanceID;
	#endif
};

// Per-pixel color data passed through the pixel shader.
struct GeometryShaderOutput
{
	float4 pos : SV_POSITION;
	#ifndef BYPASS_PIXEL_SHADER
		snorm float2 colorLookupUV : TEXCOORD_0;
		float2 parallaxUV : TEXCOORD_1_FB_MSAA;
		float4 encodedPlane : PLANE_INFO;
		#ifdef FOG
			float4 fogColor : FOG_COLOR;
		#endif
	#endif
	#ifdef INSTANCEDSTEREO
		uint renTarget_id : SV_RenderTargetArrayIndex;
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
			#ifndef BYPASS_PIXEL_SHADER
				output.colorLookupUV = input[j].colorLookupUV;
				output.parallaxUV = input[j].parallaxUV;
				output.encodedPlane = input[j].encodedPlane;
				#ifdef FOG
					output.fogColor = input[j].fogColor;
				#endif
			#endif

			#ifdef INSTANCEDSTEREO
				output.renTarget_id = i;
			#endif
			outStream.Append(output);
		}
	}
}