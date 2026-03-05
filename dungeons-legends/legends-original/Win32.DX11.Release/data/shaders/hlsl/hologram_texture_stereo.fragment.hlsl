// DO NOT HAVE THIS INCLUDE BE THE FIRST LINE!  we need a #define to be respected in the .fxh file
#include "ShaderConstants.fxh"

struct PS_Input
{
	float4	position		: SV_Position;
	float2	uv				: TEXCOORD_0; // the stereo texture uses a texture array
#ifdef INSTANCEDSTEREO
	uint	instanceID		: SV_InstanceID;
#endif
};

struct PS_Output
{
	float4 color : SV_Target;
};

void main( in PS_Input PSInput, out PS_Output PSOutput )
{
#ifdef INSTANCEDSTEREO
	float3 uvr = float3(PSInput.uv, PSInput.instanceID); // 3rd element selects the texture
#else
	int eyeIndex = 0;	//TODO: Right now this code will render mono if we were to implement non-instanced stereo rendering
	float3 uvr = float3(PSInput.uv, eyeIndex); // 3rd element selects the texture
#endif
	PSOutput.color = TEXTURE_0.Sample(TextureSampler0, uvr);
}