#include "ShaderConstants.fxh"
#include "Util.fxh"

struct PS_Input
{
	float4 position : SV_Position;

#ifndef BYPASS_PIXEL_SHADER
	lpfloat4 color : COLOR;
	snorm float2 uv0 : TEXCOORD_0_FB_MSAA;
	snorm float2 uv1 : TEXCOORD_1_FB_MSAA;
#endif

#ifdef NEAR_WATER
	float cameraDist : TEXCOORD_2;
#endif

#ifdef FOG
	float4 fogColor : FOG_COLOR;
#endif
};

struct PS_Output
{
	float4 color : SV_Target;
};

void main( in PS_Input PSInput, out PS_Output PSOutput )
{
	float4 diffuse = TEXTURE_0.Sample(TextureSampler0, PSInput.uv0);

#ifdef SEASONS_FAR
	diffuse.a = 1.0f;
	PSInput.color.b = 1.0f;
#endif

	//If we know that all the verts in a triangle will have the same alpha, we should cull there first.
	if (diffuse.a <= 0.0f) {
		discard;
	}

	diffuse = diffuse * TEXTURE_1.Sample(TextureSampler1, PSInput.uv1);

#ifndef SEASONS

	diffuse.rgb *= lerp(1.0f, PSInput.color.rgb, diffuse.a);
	diffuse.a = 1.0f;

#else
	float2 uv = PSInput.color.xy;
	PSInput.color.b = lerp(0.0f, PSInput.color.b, diffuse.a);
	diffuse.rgb *= lerp(1.0f, TEXTURE_2.Sample(TextureSampler2, uv).rgb*2.0f, PSInput.color.b);
	diffuse.rgb *= PSInput.color.aaa;
	diffuse.a = 1.0f;
#endif

#ifdef FOG
	diffuse.rgb = lerp( diffuse.rgb, PSInput.fogColor.rgb, PSInput.fogColor.a );
#endif

	PSOutput.color = diffuse;

#ifdef VR_MODE
	// On Rift, the transition from 0 brightness to the lowest 8 bit value is abrupt, so clamp to 
	// the lowest 8 bit value.
	PSOutput.color = max(PSOutput.color, 1 / 255.0f);
#endif

}