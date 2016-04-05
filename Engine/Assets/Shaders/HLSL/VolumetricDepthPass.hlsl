#include "Uniforms.hlsl"
#include "Transform.hlsl"
#include "Samplers.hlsl"
#include "ScreenPos.hlsl"

const float2 PixelSize = float2(1.0f / 1600.0f, 1.0f / 900.0f); //RT width / RT Height
const float2 HalfPixel = float2(1.0f / 1600.0f, 1.0f / 900.0f) * 0.5f;
const float Scale = 4;
const float Intensity = 1;
const float Spread = 0.005;
const float Decay = 0.5f;
const float4 ShaftTint = float4(1,1,1,1);
const float TextureAspectRatio = 0.5625f;

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
  const float2 PixelSize = float2(1.0f / 1600.0f, 1.0f / 900.0f); //RT width / RT Height
  const float Scale = 4;
	float depth = Sample2D(EmissiveMap,iScreenPos).r;
	float3 color = Sample2D(DiffMap,iScreenPos).rgb;
	color += Sample2D(DiffMap,iScreenPos + float2(0,PixelSize.y)).rgb;
	color += Sample2D(DiffMap,iScreenPos + float2(PixelSize.x,0)).rgb;
	color += Sample2D(DiffMap,iScreenPos + PixelSize).rgb;
	oColor = float4(color*0.25f*Scale, depth);
}
