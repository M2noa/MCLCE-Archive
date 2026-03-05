#include "ShaderConstants.fxh"

struct GeometryShaderInput
{
	float4	pos				: SV_POSITION;
#ifdef INSTANCEDSTEREO
	uint instanceID : SV_InstanceID;
#endif
};

// Per-pixel color data passed through the pixel shader.
struct GeometryShaderOutput
{
	float4	pos				: SV_POSITION;
#ifdef INSTANCEDSTEREO
	uint        renTarget_id : SV_RenderTargetArrayIndex;
#endif
};

// passes through the triangles, except changint the viewport id to match the instance
#ifdef LINE_STRIP
[maxvertexcount(2)]
void main(line GeometryShaderInput input[2], inout LineStream<GeometryShaderOutput> outStream)
#else
[maxvertexcount(3)]
void main(triangle GeometryShaderInput input[3], inout TriangleStream<GeometryShaderOutput> outStream)
#endif
{
	GeometryShaderOutput output = (GeometryShaderOutput)0;

#ifdef INSTANCEDSTEREO
	int i = input[0].instanceID;
#endif
#ifdef LINE_STRIP
	for (int j = 0; j < 2; j++)
#else
	for (int j = 0; j < 3; j++)
#endif
	{
		output.pos = input[j].pos;
#ifdef INSTANCEDSTEREO
		output.renTarget_id = i;
#endif
		outStream.Append(output);
	}
}