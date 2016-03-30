
// Simple copy or blend of the ambient occlusion buffer.
// Only for testing, ambient occlusion should only affect ambient light.

#include "Uniforms.hlsl"
#include "Samplers.hlsl"
#include "Transform.hlsl"
#include "ScreenPos.hlsl"

#line 10

void VS(float4 iPos : POSITION,
    out float2 oScreenPos : TEXCOORD0,
    out float4 oPos : POSITION)
{
    float4x3 modelMatrix = iModelMatrix;
    float3 worldPos = GetWorldPos(modelMatrix);
    oPos = GetClipPos(worldPos);
    oScreenPos = GetScreenPosPreDiv(oPos);
}

void PS(float2 iScreenPos : TEXCOORD0,
    out float4 oColor : OUTCOLOR0)
{
    float occlusion = Sample2D(EmissiveMap, iScreenPos).r;
#ifdef AO_ONLY
    float3 color = float3(occlusion,occlusion,occlusion);
#else
    float3 color = Sample2D(DiffMap, iScreenPos).rgb * occlusion;
#endif
    oColor = float4(color, 1.0);
}
