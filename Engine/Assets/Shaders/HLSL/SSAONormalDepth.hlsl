#include "Uniforms.hlsl"
#include "Samplers.hlsl"
#include "Transform.hlsl"
#include "ScreenPos.hlsl"

void VS(float4 iPos : POSITION,
        float3 iNormal : NORMAL,
        float2 iTexCoord : TEXCOORD0,
        out float2 oTexCoord : TEXCOORD0,
        out float3 oNormal : TEXCOORD1,
        out float3 oWorldPos : TEXCOORD2,
        out float3 oVertexLight : TEXCOORD4,
        out float4 oScreenPos : TEXCOORD5,
        out float4 oPos : OUTPOSITION)
{
  float4x3 modelMatrix = iModelMatrix;
  float3 worldPos = GetWorldPos(modelMatrix);
  oPos = GetClipPos(worldPos);
  oNormal = GetWorldNormal(modelMatrix);
  oWorldPos = worldPos;
  oTexCoord = GetTexCoord(iTexCoord);
  oVertexLight = float3(1,1,1);
  oScreenPos = GetScreenPos(oPos);

  oNormal = mul(oNormal,(float3x3)cView);
  oWorldPos = mul(float4(oWorldPos,1.0f),cView);
}

void PS(
  float2 iTexCoord : TEXCOORD0,
  float3 iNormal : TEXCOORD1,
  float3 iWorldPos : TEXCOORD2,
  float3 iVertexLight : TEXCOORD4,
  float4 iScreenPos : TEXCOORD5,
  out float4 oColor : OUTCOLOR0)
{
  float3 normal = normalize(iNormal);
  oColor = float4(normal,iWorldPos.z);
}
