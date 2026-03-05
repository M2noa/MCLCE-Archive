#include "ShaderConstants.fxh"
#include "util.fxh"

struct PS_Input {
	float4 position : SV_Position;

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

struct PS_Output {
	float4 color : SV_Target;
};

static const float MAX_LAYER_DEPTH = 32.0;

void main( in PS_Input PSInput, out PS_Output PSOutput ) {
	#ifdef BYPASS_PIXEL_SHADER
		PSOutput.color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	#else
		///// Decode Input Values
		// Decode parallax plane data
		// Using round() because normals should all be in the standard basis
		const float planeDistance = PSInput.encodedPlane.a * MAX_LAYER_DEPTH;

		///// Color Assembly
		float4 colorSample = TEXTURE_1.Sample(TextureSampler1, PSInput.colorLookupUV);
		float4 textureSample = TEXTURE_0.Sample(TextureSampler0, PSInput.parallaxUV);
		const float3 brightness = textureSample.rgb * (1.0 - PSInput.encodedPlane.w);
		colorSample.rgb *= brightness;

		// Look for hard-coded value to clear the portal first
		#ifdef FOG
			if(planeDistance > MAX_LAYER_DEPTH - 1.0) {
				PSOutput.color = float4(PSInput.fogColor.rgb * PSInput.fogColor.a, 0.0f);
			}
			else {
				PSOutput.color = float4(colorSample.rgb * (1.0 - PSInput.fogColor.a), 1.0f);
			}
		#else
			if(planeDistance > MAX_LAYER_DEPTH - 1.0) {
				PSOutput.color = float4(0.0f, 0.0f, 0.0f, 0.0f);
			}
			else {
				PSOutput.color = float4(colorSample.rgb, 1);
			}
		#endif
	#endif
}
