#include "Uniforms.hlsl"
#include "Transform.hlsl"
#include "Samplers.hlsl"
#include "ScreenPos.hlsl"


void VS(float4 iPos : POSITION,
	out float2 oTexCoord : TEXCOORD0,
	out float2 oScreenPos : TEXCOORD1,
	out float4 oPos : OUTPOSITION)
{
    float4x3 modelMatrix = iModelMatrix;
    float3 worldPos = GetWorldPos(modelMatrix);
    oPos = GetClipPos(worldPos);
    oTexCoord = GetQuadTexCoord(oPos);
    oScreenPos = GetScreenPosPreDiv(oPos);
}

void PS(float2 iTexCoord : TEXCOORD0,
    float2 iScreenPos : TEXCOORD1,
    out float4 oColor : OUTCOLOR0)
{
  //Top left
if (iScreenPos.x < 0.5f && iScreenPos.y < 0.5f)
  oColor = Sample2D(AlbedoBuffer, iScreenPos*2);
////Top right
if (iScreenPos.x > 0.5 && iScreenPos.y < 0.5)
  oColor = Sample2D(NormalBuffer, float2(iScreenPos.x*2 - 1,iScreenPos.y*2));
////Bottom left
if (iScreenPos.x < 0.5 && iScreenPos.y > 0.5)
{
  float depth = 1 - Sample2D(DepthBuffer, float2(iScreenPos.x * 2,iScreenPos.y*2 - 1)).r;
	oColor = float4(depth, depth, depth, depth);
}
////Bottom right
if (iScreenPos.x > 0.5 && iScreenPos.y > 0.5)
  oColor = Sample2D(EmissiveMap, iScreenPos*2 -1);
}
