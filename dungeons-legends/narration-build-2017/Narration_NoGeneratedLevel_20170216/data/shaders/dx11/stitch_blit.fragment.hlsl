#include "ShaderConstants.fxh"
#include "Util.fxh"

#ifndef BLUR_RADIUS
    #define BLUR_RADIUS 1 //base size
#endif

//HACK do not build higher res versions on DX9
#if VERSION < 0xa000 && BLUR_RADIUS > 2
    #undef BLUR_RADIUS
    #define BLUR_RADIUS 2
#endif

struct PS_Input
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD_0;
};

struct PS_Output
{
    float4 color : SV_Target;
};

void main( in PS_Input PSInput, out PS_Output PSOutput )
{
    float2 pixelSide = float2(1.0 / TEXTURE_DIMENSIONS.xy);

    #if BLUR_RADIUS == 1
        float4 color = TEXTURE_0.Sample(TextureSampler0, PSInput.uv);
    #else
        float4 color = float4(0, 0, 0, 0);
        for(int i = -BLUR_RADIUS / 2; i < BLUR_RADIUS / 2; ++i) {
            for(int j = -BLUR_RADIUS / 2; j < BLUR_RADIUS / 2; ++j) {
                float2 uv = PSInput.uv + pixelSide * float2(j, i);
                uv = fmod(uv, float2(1,1));
    
                color += TEXTURE_0.Sample(TextureSampler0, uv);
            }
        }

        color /= BLUR_RADIUS * BLUR_RADIUS;
    #endif  

    color.rgb = lerp(color.rgb, CURRENT_COLOR.rgb * color.rgb, color.a * CURRENT_COLOR.a);
    if(CURRENT_COLOR.a > 0) {
        //the alpha in diffuse is a mask
        color.a = 1;
    }

    PSOutput.color = color;
}