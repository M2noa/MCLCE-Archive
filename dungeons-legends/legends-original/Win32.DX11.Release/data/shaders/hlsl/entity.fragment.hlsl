#include "ShaderConstants.fxh"
#include "util.fxh"

struct PS_Input {
	float4 position : SV_Position;

	float4 light : LIGHT;
	float4 fogColor : FOG_COLOR;

#ifdef GLINT
	// there is some alignment issue on the Windows Phone 1320 that causes the position
	// to get corrupted if this is two floats and last in the struct memory wise
	float4 layerUV : GLINT_UVS;
#endif

#ifdef USE_OVERLAY
	float4 overlayColor : OVERLAY_COLOR;
#endif

	float4 teamColor : TEAM_COLOR;

#ifdef TINTED_ALPHA_TEST
	float4 alphaTestMultiplier : ALPHA_MULTIPLIER;
#endif

	float2 uv : TEXCOORD_0_FB_MSAA;

};

struct PS_Output
{
	float4 color : SV_Target;
};

#ifdef USE_EMISSIVE
#ifdef USE_ONLY_EMISSIVE
#define NEEDS_DISCARD(C) (C.a == 0.0f ||C.a == 1.0f )
#else
#define NEEDS_DISCARD(C)	(C.a + C.r + C.g + C.b == 0.0)
#endif
#else
#ifndef USE_COLOR_MASK
#define NEEDS_DISCARD(C)	(C.a < 0.5)
#else
#define NEEDS_DISCARD(C)	(C.a == 0.0)
#endif
#endif

float4 glintBlend(float4 dest, float4 source) {
	// glBlendFuncSeparate(GL_SRC_COLOR, GL_ONE, GL_ONE, GL_ZERO)
	return float4(source.rgb * source.rgb, source.a) + float4(dest.rgb, 0.0);
}

float3 blendTeamColor(float3 base, float3 blend, float alpha, float4 mask)
{
	float3 clr = base;

	// mask textures should have an alpha of 1, the _nomask texture has an alpha of 0
	if (mask.a < 0.5f)
	{
		clr += blend * alpha;
	}
	else
	{
		clr = lerp(clr, base * blend, mask.r);	// mask.r = multiply
		clr = clr + (blend * mask.g);			// mark.g = additive
		clr = lerp(clr, blend, mask.b);			// mask.b = linear blend
	}

	return clr;
}

void main( in PS_Input PSInput, out PS_Output PSOutput )
{
	float4 color = float4( 1.0f, 1.0f, 1.0f, 1.0f );
	float4 teamMask = float4(0.f, 0.f, 0.f, 0.f);

#if( !defined(NO_TEXTURE) || !defined(COLOR_BASED) || defined(USE_COLOR_BLEND) )

#if !defined(TEXEL_AA) || !defined(TEXEL_AA_FEATURE) || (VERSION < 0xa000 /*D3D_FEATURE_LEVEL_10_0*/)
	color = TEXTURE_0.Sample( TextureSampler0, PSInput.uv );
#else
	color = texture2D_AA(TEXTURE_0, TextureSampler0, PSInput.uv);
#endif

	teamMask = TEXTURE_3.Sample(TextureSampler0, PSInput.uv);

#if defined(ALPHA_TEST) && !defined(USE_MULTITEXTURE)
	if( NEEDS_DISCARD( color ) )
	{
		discard;
	}
#endif

#ifdef TINTED_ALPHA_TEST
	float4 testColor = color;
	testColor.a = testColor.a * PSInput.alphaTestMultiplier.r;
	if( NEEDS_DISCARD( testColor ) )
	{
		discard;
	}
#endif

#endif

#ifdef USE_COLOR_MASK
	color.rgb = lerp( color, color * CHANGE_COLOR, color.a ).rgb;
	color.a *= CHANGE_COLOR.a;
#endif

#ifdef ITEM_IN_HAND
	color.rgb = lerp(color, color * CHANGE_COLOR, PSInput.light.a).rgb;
#endif

#ifdef USE_MULTITEXTURE
	float4 tex1 = TEXTURE_1.Sample(TextureSampler1, PSInput.uv);
	float4 tex2 = TEXTURE_2.Sample(TextureSampler2, PSInput.uv);
	color.rgb = lerp(color.rgb, tex1, tex1.a);
#ifdef ALPHA_TEST
	if (color.a < 0.5f && tex1.a == 0.0f) {
		discard;
	}
#endif

#ifdef COLOR_SECOND_TEXTURE
	if (tex2.a > 0.0f) {
		color.rgb = lerp(tex2.rgb, tex2 * CHANGE_COLOR, tex2.a);
	}
#else
	color.rgb = lerp(color.rgb, tex2, tex2.a);
#endif
#endif

#ifdef USE_OVERLAY
	//use either the diffuse or the OVERLAY_COLOR
	color.rgb = lerp( color, PSInput.overlayColor, PSInput.overlayColor.a ).rgb;
#endif

	color.rgb = blendTeamColor(color.rgb, PSInput.teamColor.rgb, PSInput.teamColor.a, teamMask);

#ifdef USE_EMISSIVE
	//make glowy stuff
	color *= lerp( float( 1.0 ).xxxx, PSInput.light, color.a );
#else
	color *= PSInput.light;
#endif

	//apply fog
	color.rgb = lerp( color.rgb, PSInput.fogColor.rgb, PSInput.fogColor.a );

#ifdef GLINT
	// Applies color mask to glint texture instead and blends with original color
	float4 layer1 = TEXTURE_1.Sample(TextureSampler1, frac(PSInput.layerUV.xy)).rgbr * GLINT_COLOR;
	float4 layer2 = TEXTURE_1.Sample(TextureSampler1, frac(PSInput.layerUV.zw)).rgbr * GLINT_COLOR;
	float4 glint = (layer1 + layer2) * TILE_LIGHT_COLOR;
	color = glintBlend(color, glint);
#endif

	//WARNING do not refactor this 
	PSOutput.color = color;

#ifdef VR_MODE
	// On Rift, the transition from 0 brightness to the lowest 8 bit value is abrupt, so clamp to 
	// the lowest 8 bit value.
	PSOutput.color = max(PSOutput.color, 1 / 255.0f);
#endif
}