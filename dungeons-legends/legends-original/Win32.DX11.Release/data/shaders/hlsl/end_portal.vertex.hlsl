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

	#ifndef BYPASS_PIXEL_SHADER
		snorm float2 colorLookupUV : TEXCOORD_0_FB_MSAA;
		float2 parallaxUV : TEXCOORD_1_FB_MSAA;
		float4 encodedPlane : PLANE_INFO;

		// lpfloat4 color : COLOR;

		// float3 eyePositionInWorld : EYE_POS;
		// float3 surfacePositionInWorld : SURFACE_POS;

		#ifdef FOG
			float4 fogColor : FOG_COLOR;
		#endif
	#endif

	#ifdef GEOMETRY_INSTANCEDSTEREO
		uint instanceID : SV_InstanceID;
	#endif
	#ifdef VERTEXSHADER_INSTANCEDSTEREO
		uint renTarget_id : SV_RenderTargetArrayIndex;
	#endif
};

static const float DIST_DESATURATION = 56.0 / 255.0; //WARNING this value is also hardcoded in the water color, don'tchange

static const float MAX_LAYER_DEPTH = 32.0;

void main(in VS_Input VSInput, out PS_Input PSInput) {
	///// Vertex Transformation
	float3 worldPos = (VSInput.position.xyz * CHUNK_ORIGIN_AND_SCALE.w) + CHUNK_ORIGIN_AND_SCALE.xyz;
	float3 viewRay = worldPos.xyz;

	#ifdef INSTANCEDSTEREO
		int i = VSInput.instanceID;

		PSInput.position = mul(WORLDVIEW_STEREO[i], float4(worldPos, 1 ));
		PSInput.position = mul(PROJ_STEREO[i], PSInput.position);

		viewRay -= WORLDVIEW_STEREO[i]._m30_m31_m32;

	#else
		PSInput.position = mul(WORLDVIEW, float4( worldPos, 1 ));
		PSInput.position = mul(PROJ, PSInput.position);
	#endif
	#ifdef GEOMETRY_INSTANCEDSTEREO
		PSInput.instanceID = VSInput.instanceID;
	#endif 
	#ifdef VERTEXSHADER_INSTANCEDSTEREO
		PSInput.renTarget_id = VSInput.instanceID;
	#endif

	#ifndef BYPASS_PIXEL_SHADER
		PSInput.encodedPlane = VSInput.color;
		PSInput.colorLookupUV = VSInput.uv0;

		///// Decode Input Values
		// Decode parallax plane data
		const float4 planeData = (PSInput.encodedPlane - float4(0.5, 0.5, 0.5, 0.0)) * float4(2.0, 2.0, 2.0, MAX_LAYER_DEPTH);
		const float3 planeNormal = planeData.rgb;
		const float planeDistance = planeData.a;
		

		///// Ray-cast for parallax-offset UV
		// Perform ray-plane intersection to find the position on the parallax plane
		float t = dot(viewRay - (planeNormal * planeDistance), planeNormal) / dot(viewRay, planeNormal);
		float3 parallaxPositionInWorld = (t * viewRay) + VIEW_POS;

		///// Ridiculous UV-remapping
		const float3 normalMask = abs(planeNormal);
		// Since all normals are orthonormal on <x,y,z>, mask out the correct uv result
		float2 raycastUV = parallaxPositionInWorld.yz * normalMask[0];
		raycastUV += parallaxPositionInWorld.xz * normalMask[1];
		raycastUV += parallaxPositionInWorld.xy * normalMask[2];
		// Scale the UVs to Minecraft pixel size
		raycastUV = raycastUV / 16.0;

		///// Color Lookup
		const float rotor = 3.1415926535897 * (5.0 / 7.0);
		float rotS = sin(planeDistance * rotor);
		float rotC = cos(planeDistance * rotor);

		PSInput.parallaxUV = mul(float2x2(float2(rotC, rotS), float2(-rotS, rotC)), raycastUV);

		///// UV Scrolling
		// Offset rotation based on a value unique to the layer
		PSInput.parallaxUV += float2(rotC, rotS) * planeDistance;
		PSInput.parallaxUV.y += TIME / 256.0;

		const float modVal = 64.0;
		PSInput.parallaxUV = fmod(PSInput.parallaxUV, modVal);

		///// Fog
		#ifdef FOG
			#ifdef FANCY
				float3 relPos = -worldPos;
				float cameraDepth = length(relPos);
			#else
				float cameraDepth = PSInput.position.z;
			#endif
			float len = cameraDepth / RENDER_DISTANCE;
			#ifdef ALLOW_FADE
				len += RENDER_CHUNK_FOG_ALPHA.r;
#endif
			PSInput.fogColor.rgb = FOG_COLOR.rgb;
			PSInput.fogColor.a = clamp((len - FOG_CONTROL.x) / (FOG_CONTROL.y - FOG_CONTROL.x), 0.0, 1.0);
		#endif
	#endif
}
