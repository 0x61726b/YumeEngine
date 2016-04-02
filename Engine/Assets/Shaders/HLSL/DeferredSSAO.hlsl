#include "Uniforms.hlsl"
#include "Samplers.hlsl"
#include "Transform.hlsl"
#include "ScreenPos.hlsl"

#ifdef COMPILEVS

#else
SamplerState samRandomVec
{
	Filter = MIN_MAG_LINEAR_MIP_POINT;
	AddressU  = WRAP;
    AddressV  = WRAP;
};
#endif

void VS(float4 iPos : POSITION,
        float2 iTexCoord : TEXCOORD0,
    out float2 oTexCoord : TEXCOORD0,
    out float2 oScreenPos : TEXCOORD1,
    out float4 oPos : OUTPOSITION)
{
    float4x3 modelMatrix = iModelMatrix;
    float3 worldPos = GetWorldPos(modelMatrix);
    oPos = GetClipPos(worldPos);
    oTexCoord = iTexCoord;
    oScreenPos = GetScreenPosPreDiv(oPos);
}
Texture2D tRandomVecMap : register(t6);

void PS(float2 iTexCoord : TEXCOORD0,
        float2 iScreenPos : TEXCOORD1,
    out float4 oColor : OUTCOLOR0)
{
  oColor = float4(iScreenPos,0,0);
}
