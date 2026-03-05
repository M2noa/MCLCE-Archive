// These [aren't but] should be grouped in a way that they require the least amount of updating (world data in one, model data in another, part of model data in another one, etc)

#if (defined(USE_STEREO_TEXTURE_ARRAY) || defined(ARRAY_TEXTURE_0)) && (VERSION >= 0xa000)
Texture2DArray TEXTURE_0 : register (t0);
#else
Texture2DMS<float4> TEXTURE_0_MS : register(t0);
Texture2D TEXTURE_0 : register(t0);
#endif

Texture2DMS<float4> TEXTURE_1_MS : register(t1);
Texture2D TEXTURE_1 : register(t1);
Texture2D TEXTURE_2 : register(t2);
#ifndef NO_TEX_3
Texture2D TEXTURE_3 : register(t3);
sampler TextureSampler3 : register(s3);
#endif

// Make sure this thing is actually getting bound
sampler TextureSampler0 : register(s0);
sampler TextureSampler1 : register(s1);
sampler TextureSampler2 : register(s2);

#ifdef LOW_PRECISION
#define lpfloat min16float
#define lpfloat2 min16float2
#define lpfloat4 min16float4
#else
#define lpfloat float
#define lpfloat2 float2
#define lpfloat4 float4
#endif

#if defined(MSAA_FRAMEBUFFER_ENABLED)
#define TEXCOORD_0_FB_MSAA TEXCOORD_0_centroid
#define TEXCOORD_1_FB_MSAA TEXCOORD_1_centroid
#define TEXCOORD_2_FB_MSAA TEXCOORD_2_centroid
#define TEXCOORD_3_FB_MSAA TEXCOORD_3_centroid
#else
#define TEXCOORD_0_FB_MSAA TEXCOORD_0
#define TEXCOORD_1_FB_MSAA TEXCOORD_1
#define TEXCOORD_2_FB_MSAA TEXCOORD_2
#define TEXCOORD_3_FB_MSAA TEXCOORD_3
#endif


cbuffer RenderChunkConstants : register(b0)
{
    float4 CHUNK_ORIGIN_AND_SCALE;
    float RENDER_CHUNK_FOG_ALPHA;
}


cbuffer EntityConstants
{
    float4 OVERLAY_COLOR;
    float4 TILE_LIGHT_COLOR;
    float4 CHANGE_COLOR;
    float4 GLINT_COLOR;
	float4 TEAM_COLOR;
    float4 UV_ANIM;
    float2 UV_OFFSET;
    float2 UV_ROTATION;
    float2 GLINT_UV_SCALE;
}

cbuffer PerFrameConstants : register(b2)
{

    float3 VIEW_POS;
    float TIME;

    float4 FOG_COLOR;

    float2 FOG_CONTROL;

    float RENDER_DISTANCE;
    float FAR_CHUNKS_DISTANCE;
}


#if !defined(INSTANCEDSTEREO)

cbuffer WorldConstants : register(b1)
{
    float4x4 WORLDVIEWPROJ;
    float4x4 WORLD;
    float4x4 WORLDVIEW;
    float4x4 PROJ;
}

#else

cbuffer WorldConstantsStereographic {
	float4x4 WORLDVIEWPROJ_STEREO[2];
	float4x4 WORLD_STEREO;
	float4x4 WORLDVIEW_STEREO[2];
	float4x4 PROJ_STEREO[2];
}

#endif

cbuffer ShaderConstants : register(b3)
{
    float4 CURRENT_COLOR;
    float4 DARKEN;
    float3 TEXTURE_DIMENSIONS;
    float1 HUD_OPACITY;
    float4x4 UV_TRANSFORM;
	int MSAA_SAMPLECOUNT;
}

cbuffer WeatherConstants
{
    float4 POSITION_OFFSET;
    float4 VELOCITY;
    float4 ALPHA;
    float4 VIEW_POSITION;
    float4 SIZE_SCALE;
    float4 FORWARD;
    float4 UV_INFO;
    float4 PARTICLE_BOX;
}

cbuffer FlipbookTextureConstants
{
    float1 V_OFFSET;
    float1 V_BLEND_OFFSET;
}

cbuffer EffectsConstants
{
    float2 EFFECT_UV_OFFSET;
}

cbuffer BannerConstants
{
    float4 BANNER_COLORS[7];
    float4 BANNER_UV_OFFSETS[7];
}

cbuffer TextConstants
{
    float1 GLYPH_SMOOTH_RADIUS;
    float1 GLYPH_CUTOFF;
    float1 OUTLINE_CUTOFF;
    float4 OUTLINE_COLOR;
    float1 SHADOW_SMOOTH_RADIUS;
    float4 SHADOW_COLOR;
    float2 SHADOW_OFFSET;
}
cbuffer DebugConstants
{
    float TEXTURE_ARRAY_INDEX_0;
};

cbuffer InterFrameConstants
{
	// in secs. This is reset every 2 mins. so the range is [0, 120)
	// make sure your shader handles the case when it transitions from 120 to 0
    float TOTAL_REAL_WORLD_TIME;
    float4x4 CUBE_MAP_ROTATION;
};

cbuffer PostProcessConstants {
	int GaussianBlurSize;
	float DepthOfFieldNearEndDepth;
	float DepthOfFieldFarStartDepth;
	float DepthOfFieldFarEndDepth;
};

cbuffer BadgerConstants {
	int NUM_STRUCTURES;
	float4 STRUCTURE_DATA[8];
	float4 STRUCTURE_COLOR[8];
}
