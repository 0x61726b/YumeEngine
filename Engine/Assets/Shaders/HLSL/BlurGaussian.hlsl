
// Gaussian filter using texture bilinear interpolation.
// Based on a paper by Daniel Rákos:
// http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling

#include "Uniforms.hlsl"
#include "Samplers.hlsl"
#include "Transform.hlsl"
#include "ScreenPos.hlsl"

#line 11

void VS(float4 iPos : POSITION,
    out float2 oTexCoord : TEXCOORD0,
    out float4 oPos : OUTPOSITION)
{
    float4x3 modelMatrix = iModelMatrix;
    float3 worldPos = GetWorldPos(modelMatrix);
    oPos = GetClipPos(worldPos);
    oTexCoord = GetQuadTexCoord(oPos);
}

#ifdef VERTICAL
    #define AXIS y
    #define DIR yx
#else
    #define AXIS x
    #define DIR xy
#endif

void PS(float2 iTexCoord : TEXCOORD0,
    out float4 oColor : OUTCOLOR0)
{
    // 2+1+2 samples
#if SAMPLES==2
    //float2 weight = float2( 0.29412, 0.35294 );                         //sigma = 1.483
    //float2 offset = float2( 0.0, 1.3333 ) * cGBufferInvSize.AXIS;
    // sigma = 1.5
    // gauss = 0.292082 0.233881 0.120078
    float2 weight = float2( 0.292082 0.353959 );
    float2 offset = float2( 0.0, 1.33924 ) * cGBufferInvSize.AXIS;

    oColor  = Sample2D(DiffMap, iTexCoord - float2(offset.y, 0.0).DIR) * weight.y;
    oColor += Sample2D(DiffMap, iTexCoord                            ) * weight.x;
    oColor += Sample2D(DiffMap, iTexCoord + float2(offset.y, 0.0).DIR) * weight.y;

    // 4+1+4 samples
#elif SAMPLES==4
    //vec3 weight = vec3( 0.227027, 0.316217, 0.070270 );           //sigma = 1.7754
    //vec3 offset = vec3( 0.0, 1.3846, 3.2308 ) * cGBufferInvSize.AXIS;
    //vec3 weight = vec3( 0.250404, 0.320621, 0.0541771 );          //sigma = 1.6
    //vec3 offset = vec3( 0.0, 1.35757, 3.20307 ) * cGBufferInvSize.AXIS;
    // sigma = 2
    // gauss = 0.204164 0.180174 0.123832 0.0662822 0.0276306
    float4 weight = float4( 0.204164, 0.304005, 0.0939128 );
    float4 offset = float4( 0.0, 1.40733, 3.29421 ) * cGBufferInvSize.AXIS;

    oColor  = Sample2D(DiffMap, iTexCoord - float2(offset.z, 0.0).DIR) * weight.z;
    oColor += Sample2D(DiffMap, iTexCoord - float2(offset.y, 0.0).DIR) * weight.y;
    oColor += Sample2D(DiffMap, iTexCoord                            ) * weight.x;
    oColor += Sample2D(DiffMap, iTexCoord + float2(offset.y, 0.0).DIR) * weight.y;
    oColor += Sample2D(DiffMap, iTexCoord + float2(offset.z, 0.0).DIR) * weight.z;

    // 6+1+6 samples
#elif SAMPLES==6
    //float4 weight = float4( 0.18571, 0.28870, 0.10364, 0.014805 );        //sigma = 2.15327
    //float4 offset = float4( 0.0, 1.4211, 3.3158, 5.2105 ) * cGBufferInvSize.AXIS;
    //float4 weight = float4( 0.167292, 0.271598, 0.118306, 0.0264486 );    //sigma = 2.4
    //float4 offset = float4( 0.0, 1.4353, 3.3526, 5.2779 ) * cGBufferInvSize.AXIS;
    // sigma = 3.5
    // gauss = 0.121569 0.116706 0.103256 0.0841947 0.0632704 0.0438191 0.0279688
    float4 weight = float4( 0.121569, 0.219963, 0.147465, 0.071788 );
    float4 offset = float4( 0.0, 1.46943, 3.42905, 5.3896 ) * cGBufferInvSize.AXIS;

    oColor  = Sample2D(DiffMap, iTexCoord - float2(offset.w, 0.0).DIR) * weight.w;
    oColor += Sample2D(DiffMap, iTexCoord - float2(offset.z, 0.0).DIR) * weight.z;
    oColor += Sample2D(DiffMap, iTexCoord - float2(offset.y, 0.0).DIR) * weight.y;
    oColor += Sample2D(DiffMap, iTexCoord                            ) * weight.x;
    oColor += Sample2D(DiffMap, iTexCoord + float2(offset.y, 0.0).DIR) * weight.y;
    oColor += Sample2D(DiffMap, iTexCoord + float2(offset.z, 0.0).DIR) * weight.z;
    oColor += Sample2D(DiffMap, iTexCoord + float2(offset.w, 0.0).DIR) * weight.w;

    // 8+1+8 samples
#elif SAMPLES==8
    // sigma = 5
    // gauss = 0.0875447 0.0858112 0.080814 0.0731235 0.0635705 0.0530986 0.0426126 0.0328565 0.0243407
    float weight0 = 0.087545;
    float4 weight = float4( 0.166625, 0.136694, 0.0957112, 0.0571972 );
    float4 offset = float4( 1.4850, 3.4651, 5.44522, 7.42556 ) * cGBufferInvSize.AXIS;

    oColor  = Sample2D(DiffMap, iTexCoord - float2(offset.w, 0.0).DIR) * weight.w;
    oColor += Sample2D(DiffMap, iTexCoord - float2(offset.z, 0.0).DIR) * weight.z;
    oColor += Sample2D(DiffMap, iTexCoord - float2(offset.y, 0.0).DIR) * weight.y;
    oColor += Sample2D(DiffMap, iTexCoord - float2(offset.x, 0.0).DIR) * weight.x;
    oColor += Sample2D(DiffMap, iTexCoord                            ) * weight0;
    oColor += Sample2D(DiffMap, iTexCoord + float2(offset.x, 0.0).DIR) * weight.x;
    oColor += Sample2D(DiffMap, iTexCoord + float2(offset.y, 0.0).DIR) * weight.y;
    oColor += Sample2D(DiffMap, iTexCoord + float2(offset.z, 0.0).DIR) * weight.z;
    oColor += Sample2D(DiffMap, iTexCoord + float2(offset.w, 0.0).DIR) * weight.w;
#endif
}
