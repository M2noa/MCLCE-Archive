#include "ShaderConstants.fxh"

struct VS_Input
{
    float3 position : POSITION;
    float4 color : COLOR;
#ifdef INSTANCEDSTEREO
	uint instanceID : SV_InstanceID;
#endif
};


struct PS_Input
{
    float4 position : SV_Position;
    float4 color : COLOR;
#ifdef INSTANCEDSTEREO
	uint instanceID : SV_InstanceID;
#endif
};

static const float fogNear = 0.9;

static const float3 inverseLightDirection = float3( 0.62, 0.78, 0.0 );
static const float ambient = 0.7;

void main( in VS_Input VSInput, out PS_Input PSInput )
{
#ifdef INSTANCEDSTEREO
	int i = VSInput.instanceID;
	PSInput.position = mul( WORLDVIEWPROJ_STEREO[i], float4( VSInput.position, 1 ) );
	PSInput.instanceID = i;
	float3 worldPos = mul(WORLD_STEREO, float4(VSInput.position, 1));
#else
	PSInput.position = mul(WORLDVIEWPROJ, float4(VSInput.position, 1));
	float3 worldPos = mul(WORLD, float4(VSInput.position, 1));
#endif

    PSInput.color = VSInput.color * CURRENT_COLOR;

	float depth = length(worldPos) / RENDER_DISTANCE;

    float fog = max( depth - fogNear, 0.0 );

    PSInput.color.a *= 1.0 - fog;
}