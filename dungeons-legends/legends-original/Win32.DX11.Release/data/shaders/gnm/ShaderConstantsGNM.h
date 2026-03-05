#ifndef __SHADER_CONSTANTS_GNM_H
#define __SHADER_CONSTANTS_GNM_H

#define FANCY
#define ALPHA_TEST
#define FOG
#define ENABLE_FOG
#define END_PORTAL
#define ALWAYS_LIT
//#define EFFECTS_OFFSET	- Enable this later (to see animating fire) when can turn these on/off based on material.

Texture2D TEXTURE_0 : register(t0);
Texture2D TEXTURE_1 : register(t1);
Texture2D TEXTURE_2 : register(t2);
Texture2D TEXTURE_3 : register(t3);
Texture2D TEXTURE_4 : register(t4);
Texture2D TEXTURE_5 : register(t5);
Texture2D TEXTURE_6 : register(t6);
Texture2D TEXTURE_7 : register(t7);

SamplerState TextureSampler0 : register(s0);
SamplerState TextureSampler1 : register(s1);
SamplerState TextureSampler2 : register(s2);
SamplerState TextureSampler3 : register(s3);
SamplerState TextureSampler4 : register(s4);
SamplerState TextureSampler5 : register(s5);
SamplerState TextureSampler6 : register(s6);
SamplerState TextureSampler7 : register(s7);

ConstantBuffer RenderChunkConstants : register(c0) {
	float4 CHUNK_ORIGIN_AND_SCALE;
}

ConstantBuffer EntityConstants : register(c1) {
	float4 OVERLAY_COLOR;
	float4 TILE_LIGHT_COLOR;
	float4 CHANGE_COLOR;
	float4 GLINT_COLOR;
	float4 UV_ANIM;
	float2 UV_OFFSET;
	float2 UV_ROTATION;
	float2 GLINT_UV_SCALE;
}

ConstantBuffer PerFrameConstants : register(c2) {

	float3 VIEW_POS;
	float TIME;
	float4 FOG_COLOR;
	float2 FOG_CONTROL;
	float RENDER_DISTANCE;
	float FAR_CHUNKS_DISTANCE;
}

ConstantBuffer WorldConstants : register(c3) {
	column_major float4x4 WORLDVIEWPROJ;
	column_major float4x4 WORLD;
	column_major float4x4 WORLDVIEW;
	column_major float4x4 PROJ;
}

ConstantBuffer ShaderConstants : register(c4) {
	float4 CURRENT_COLOR;
	float4 DARKEN;
	float3 TEXTURE_DIMENSIONS;
	float1 HUD_OPACITY;
	column_major float4x4 UV_TRANSFORM;
}

ConstantBuffer WeatherConstants : register(c5) {
	float4	POSITION_OFFSET;
	float4	VELOCITY;
	float4	ALPHA;
	float4	VIEW_POSITION;
	float4	SIZE_SCALE;
	float4	FORWARD;
	float4	UV_INFO;
	float4  PARTICLE_BOX;
}

ConstantBuffer FlipbookTextureConstants : register(c6) {
	float1 V_OFFSET;
	float1 V_BLEND_OFFSET;
}

ConstantBuffer EffectsConstants : register(c7) {
	float2 EFFECT_UV_OFFSET;
}

ConstantBuffer BannerConstants : register(c8) {
	float4 BANNER_COLORS[7];
	float4 BANNER_UV_OFFSETS[7];
}

ConstantBuffer InterFrameConstants : register(c9) {
	// in secs. This is reset every 1 hour. so the range is [0, 3600]
	// make sure your shader handles the case when it transitions from 3600 to 0
	float TOTAL_REAL_WORLD_TIME;
	column_major float4x4 CUBE_MAP_ROTATION;
}

#endif
