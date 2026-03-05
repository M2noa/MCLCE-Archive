// These [aren't but] should be grouped in a way that they require the least amount of updating (world data in one, model data in another, part of model data in another one, etc)

#ifdef USE_STEREO_TEXTURE_ARRAY
Texture2DArray TEXTURE_0 : register ( t0 ); 
#else
Texture2D TEXTURE_0 : register ( t0 );
#endif

Texture2D TEXTURE_1 : register ( t1 );
Texture2D TEXTURE_2 : register ( t2 );

// Make sure this thing is actually getting bound
sampler TextureSampler0 : register( s0 );
sampler TextureSampler1 : register( s1 );
sampler TextureSampler2 : register( s2 );

#ifdef LOW_PRECISION
#define lpfloat min16float
#define lpfloat2 min16float2
#define lpfloat4 min16float4
#else
#define lpfloat float
#define lpfloat2 float2
#define lpfloat4 float4
#endif

#ifdef MSAA_FRAMEBUFFER_ENABLED
#define TEXCOORD_0_FB_MSAA TEXCOORD_0_centroid
#define TEXCOORD_1_FB_MSAA TEXCOORD_1_centroid
#else
#define TEXCOORD_0_FB_MSAA TEXCOORD_0
#define TEXCOORD_1_FB_MSAA TEXCOORD_1
#endif


cbuffer RenderChunkConstants
{

	float4 CHUNK_ORIGIN_AND_SCALE;
}

cbuffer EntityConstants
{
	float4 OVERLAY_COLOR;
	float4 TILE_LIGHT_COLOR;
	float4 CHANGE_COLOR;
	float4 GLINT_COLOR;
	float2 UV_ANIM;
	float2 UV_OFFSET;
	float2 UV_ROTATION;
	float2 GLINT_UV_SCALE;
}

cbuffer PerFrameConstants
{
	float3 VIEW_DIRECTION; // unneeded
	float TIME; // unneeded

	float3 VIEW_POS;
	float FAR_CHUNKS_DISTANCE;
	float4 FOG_COLOR;

	float2 FOG_CONTROL;

	float RENDER_DISTANCE;

}


#if !defined(INSTANCEDSTEREO)

cbuffer WorldConstants
{
	float4x4 WORLDVIEWPROJ;
	float4x4 WORLD;
	float4x4 WORLDVIEW;
	float4x4 PROJ;
}

#else

cbuffer WorldConstantsStereographic
{
	float4x4 WORLDVIEWPROJ_STEREO[2];
	float4x4 WORLD_STEREO;
	float4x4 WORLDVIEW_STEREO[2];
	float4x4 PROJ_STEREO[2];
}

#endif

cbuffer ShaderConstants
{
	float4 CURRENT_COLOR;
	float4 DARKEN;
	float3 TEXTURE_DIMENSIONS;
}

cbuffer WeatherConstants {
	float4	POSITION_OFFSET;
	float4	VELOCITY;
	float4	ALPHA;
	float4	VIEW_POSITION;
	float4	SIZE_SCALE;
	float4	FORWARD;
	float4	UV_INFO;
	float4  PARTICLE_BOX;
}

cbuffer HoloConstants
{
	float4	TABLE_REVEAL_CENTER;
	float4	TABLE_REVEAL_CONSTANTS;	// x = Radius

	float3 GAZE_DIRECTION;

	float4 SKIRT_UVS;
	float4 SKIRT_UV_SCALE;

	float2 CHUNK_CLIP_MIN;
	float2 CHUNK_CLIP_MAX;

	float2 CLIP_MIN;
	float2 CLIP_MAX;
}
