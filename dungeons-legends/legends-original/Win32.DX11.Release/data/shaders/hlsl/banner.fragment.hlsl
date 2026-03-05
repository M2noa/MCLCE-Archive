#include "ShaderConstants.fxh"
#include "Util.fxh"

struct PS_Input {
	float4 position : SV_Position;
#ifdef ENABLE_LIGHT
	float4 light : LIGHT;
#endif
#ifdef ENABLE_FOG
	float4 fogColor : FOG_COLOR;
#endif

#ifndef DISABLE_TINTING
	float4 color : COLOR;
#endif

	float4 texCoords : TEXCOORD_0_FB_MSAA;
};

struct PS_Output
{
	float4 color : SV_Target;
};

void main( in PS_Input PSInput, out PS_Output PSOutput )
{
	float4 diffuse = TEXTURE_0.Sample(TextureSampler0, PSInput.texCoords.xy);
	float4 base = TEXTURE_0.Sample(TextureSampler0, PSInput.texCoords.zw);

#ifndef DISABLE_TINTING
	base.a = lerp(diffuse.r * diffuse.a, diffuse.a, PSInput.color.a);
	base.rgb *= PSInput.color.rgb;
#endif

#ifdef ENABLE_LIGHT
	base.rgb *= PSInput.light.rgb;
#endif

#ifdef ENABLE_FOG
	//apply fog
	base.rgb = lerp(base.rgb, PSInput.fogColor.rgb, PSInput.fogColor.a );
#endif

	//WARNING do not refactor this 
	PSOutput.color = base;

#ifdef VR_FEATURE
	// On Rift, the transition from 0 brightness to the lowest 8 bit value is abrupt, so clamp to 
	// the lowest 8 bit value.
	PSOutput.color = max(PSOutput.color, 1 / 255.0f);
#endif
}