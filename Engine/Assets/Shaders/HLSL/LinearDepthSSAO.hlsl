#include "Uniforms.hlsl"
#include "Samplers.hlsl"
#include "Transform.hlsl"
#include "ScreenPos.hlsl"

#ifdef COMPILEVS

float3 GetViewFarRay(float4 clipPos)
{
    return float3(cFrustumSize.x * clipPos.x,
                  cFrustumSize.y * clipPos.y,
                  cFrustumSize.z);
}


#else
#define NUM_SAMPLES 9.0
#define NUM_SPIRAL_TURNS 7.0


cbuffer CustomPS : register(b6)
{
  float4 cProjInfo;
  float cRadius;
  float cProjScale;
  float cProjScale2;
  float cBias;
  float cIntensityDivR6;
}


float2 SmallEncodeDepth(float depth)
{
    float2 ret;
    depth *= 255.0;
    ret.x = floor(depth);
    depth = (depth - ret.x) * 255.0;
    ret.y = floor(depth);
    ret.xy *= 1.0 / 255.0;
    return ret;
}

float rand(float2 co)
{
    return frac(sin(dot(co, float2(12.9898,78.233))) * 43758.5453);
}

float3 GetViewPosition(float2 ssPos, float depth, float3 iFrustumSize)
{
    // eye_z = depth(0=camera .. 1=far) * far
    float eye_z = depth * iFrustumSize.z;

    float2 t = ssPos * cProjInfo.xy + cProjInfo.zw;
    return float3(t.x, t.y, 1.0) * eye_z;
}
#endif

void VS(float4 iPos : POSITION,
	out float2 oTexCoord : TEXCOORD0,
	out float2 oScreenPos : TEXCOORD1,
	out float3 oFarRay : TEXCOORD2,
	out float3 oFrustumSize : TEXCOORD3,
	out float4 oPos : OUTPOSITION)
{
    float4x3 modelMatrix = iModelMatrix;
    float3 worldPos = GetWorldPos(modelMatrix);
    oPos = GetClipPos(worldPos);
    oScreenPos = GetScreenPosPreDiv(oPos);
    oFarRay = GetViewFarRay(oPos);
    oFrustumSize = cFrustumSize;
}
Texture2D tRandomVecMap : register(t6);

void PS(float2 iTexCoord : TEXCOORD0,
        float2 iScreenPos : TEXCOORD1,
        float3 iFrustumSize : TEXCOORD2,
    out float4 oColor : OUTCOLOR0)
{
  #ifdef SSAO_PASS
  float depthC = Sample2D(DepthBuffer, iScreenPos).r;
 float originDepth = Sample2D(DepthBuffer, iScreenPos).r;
 float3 viewPos = GetViewPosition(iScreenPos, depthC, iFrustumSize);
 float fadeStart = 100.0;
 float fadeEnd = 300.0;
 if (viewPos.z > fadeEnd)
 {
   oColor = float4(1.0, 1.0, 1.0, 1.0);
   return;
 }
 float intensity = clamp((fadeEnd - viewPos.z) / (fadeEnd - fadeStart), 0.0, 1.0) * cIntensityDivR6;
 //float3 randVec = tRandomVecMap.SampleLevel(samRandomVec, iScreenPos, 0.0f);
 //float randomAngle = randVec.x;
 float randomAngle = frac(pow(iScreenPos.x, iScreenPos.y) * 43758.5453) / cGBufferInvSize.x;
 float3 vsN = normalize(cross(ddx(viewPos), ddy(viewPos)));
 float ssDiskRadius = cProjScale2 * cProjScale * cRadius / viewPos.z;
 float sum = 0.0;
 float radius2 = cRadius * cRadius;
 const float epsilon = 0.01;
 for (float i = 0.0; i < NUM_SAMPLES; ++i)
 {
   // Screen-space radius for the tap on a unit disk
   float alpha = (i + 0.5) / NUM_SAMPLES;
   float angle = randomAngle + alpha * (NUM_SPIRAL_TURNS * 6.28);
   float2 ssOffset = floor(alpha * ssDiskRadius * float2(cos(angle), sin(angle)));
   float2 ssP = iScreenPos + ssOffset * cGBufferInvSize;
   float depthP = Sample2D(DepthBuffer, ssP).r;
   float3 vsP = GetViewPosition(ssP, depthP, iFrustumSize);
   float3 v = vsP - viewPos;
   float vv = dot(v, v);
   float vn = dot(v, vsN);
   float f = max(radius2 - vv, 0.0);
   sum += f * f * f * max((vn - cBias) / (epsilon + vv), 0.0);
 }
 float occlusion = max(0.0, 1.0 - sum * intensity * (5.0 / NUM_SAMPLES));
 oColor = float4(occlusion, occlusion,occlusion, 1.0);
 #endif

 #ifdef COMBINE
 float occlusion = Sample2D(EmissiveMap, iScreenPos).r;
 float3 color = Sample2D(DiffMap, iScreenPos).rgb * occlusion;
 oColor = float4(color,1.0f);
 #endif

 #ifdef DEBUG_AO
 float occlusion = Sample2D(EmissiveMap, iScreenPos).r;
 oColor = float4(occlusion,occlusion,occlusion,1.0f);
 #endif
}
