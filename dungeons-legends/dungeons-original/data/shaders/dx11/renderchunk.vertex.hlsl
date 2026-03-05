#include "ShaderConstants.fxh"

struct VS_Input {
	float3 position : POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD_0;
	float2 uv1 : TEXCOORD_1;
#ifdef INSTANCEDSTEREO
	uint instanceID : SV_InstanceID;
#endif
};


struct PS_Input {
	float4 position : SV_Position;
	lpfloat4 color : COLOR;
	snorm float2 uv0 : TEXCOORD_0_FB_MSAA;
	snorm float2 uv1 : TEXCOORD_1_FB_MSAA;

#ifdef NEAR_WATER
	float cameraDist : TEXCOORD_2;
#endif

#ifdef FOG
	float4 fogColor : FOG_COLOR;
#endif
#ifdef INSTANCEDSTEREO
	uint instanceID : SV_InstanceID;
#endif
};


static const float rA = 1.0;
static const float rB = 1.0;
static const float3 UNIT_Y = float3(0, 1, 0);
static const float DIST_DESATURATION = 56.0 / 255.0; //WARNING this value is also hardcoded in the water color, don'tchange


void main(in VS_Input VSInput, out PS_Input PSInput) {
	PSInput.uv0 = VSInput.uv0;
	PSInput.uv1 = VSInput.uv1;
	PSInput.color = VSInput.color;

#ifdef AS_ENTITY_RENDERER
	#ifdef INSTANCEDSTEREO
		int i = VSInput.instanceID;
		PSInput.position = mul(WORLDVIEWPROJ_STEREO[i], float4(VSInput.position, 1));
		PSInput.instanceID = i;
	#else
		PSInput.position = mul(WORLDVIEWPROJ, float4(VSInput.position, 1));
	#endif
		float3 worldPos = PSInput.position;
#else
		float3 worldPos = (VSInput.position.xyz * CHUNK_ORIGIN_AND_SCALE.w) + CHUNK_ORIGIN_AND_SCALE.xyz;
	
		// Transform to view space before projection instead of all at once to avoid floating point errors
		// Not required for entities because they are already offset by camera translation before rendering
		// World position here is calculated above and can get huge
	#ifdef INSTANCEDSTEREO
		int i = VSInput.instanceID;
	
		PSInput.position = mul(WORLDVIEW_STEREO[i], float4(worldPos, 1 ));
		PSInput.position = mul(PROJ_STEREO[i], PSInput.position);
	
		PSInput.instanceID = i;
	#else
		PSInput.position = mul(WORLDVIEW, float4( worldPos, 1 ));
		PSInput.position = mul(PROJ, PSInput.position);
	#endif
#endif

///// find distance from the camera

#if defined(FOG) || defined(NEAR_WATER)
	#ifdef FANCY
		float3 relPos = -worldPos;
		float cameraDepth = length(relPos);
		#ifdef NEAR_WATER
			PSInput.cameraDist = cameraDepth / FAR_CHUNKS_DISTANCE;
		#endif
	#else
		float cameraDepth = PSInput.position.z;
		#ifdef NEAR_WATER
			float3 relPos = -worldPos;
			float cameraDist = length(relPos);
			PSInput.cameraDist = cameraDist / FAR_CHUNKS_DISTANCE;
		#endif
	#endif
#endif

	///// apply fog

#ifdef FOG
	float len = cameraDepth / RENDER_DISTANCE;
#ifdef ALLOW_FADE
	len += CURRENT_COLOR.r;
#endif

	PSInput.fogColor.rgb = FOG_COLOR.rgb;
	PSInput.fogColor.a = clamp((len - FOG_CONTROL.x) / (FOG_CONTROL.y - FOG_CONTROL.x), 0.0, 1.0);
#endif

	///// water magic
#ifdef NEAR_WATER
#ifdef FANCY  /////enhance water
	float F = dot(normalize(relPos), UNIT_Y);
	F = 1.0 - max(F, 0.1);
	F = 1.0 - lerp(F*F*F*F, 1.0, min(1.0, cameraDepth / FAR_CHUNKS_DISTANCE));

	PSInput.color.rg -= float2(float(F * DIST_DESATURATION).xx);

	float4 depthColor = float4(PSInput.color.rgb * 0.5, 1.0);
	float4 traspColor = float4(PSInput.color.rgb * 0.45, 0.8);
	float4 surfColor = float4(PSInput.color.rgb, 1.0);

	float4 nearColor = lerp(traspColor, depthColor, PSInput.color.a);
	PSInput.color = lerp(surfColor, nearColor, F);
#else
	PSInput.color.a = PSInput.position.z / FAR_CHUNKS_DISTANCE + 0.5;
#endif //FANCY
#endif

}
