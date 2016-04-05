#include "Uniforms.hlsl"
#include "Transform.hlsl"
#include "Samplers.hlsl"
#include "ScreenPos.hlsl"

#ifdef COMPILEPS
cbuffer GodRaysPS : register(b6)
{
	float cLSSampleCount;
	float cLSWeight;
	float cLSDecay;
	float cLSExposure;
}
#endif


void VS(float4 iPos : POSITION,
	out float2 oTexCoord : TEXCOORD0,
	out float2 oScreenPos : TEXCOORD1,
  out float2 oLightScreen : TEXCOORD2,
	out float4 oPos : OUTPOSITION)
{
    float4x3 modelMatrix = iModelMatrix;
    float3 worldPos = GetWorldPos(modelMatrix);
    oPos = GetClipPos(worldPos);
    oTexCoord = GetQuadTexCoord(oPos);
    oScreenPos = GetScreenPosPreDiv(oPos);
    oLightScreen = normalize(GetScreenPosPreDiv(mul(cLightPos,cViewProj)));
}

void PS(float2 iTexCoord : TEXCOORD0,
        float2 iScreenPos : TEXCOORD1,
        float2 iLightScreen : TEXCOORD2,
    out float4 oColor : OUTCOLOR0)
{
  #ifdef GODRAY_PASS
	float4 pos = cLightPosScreen;
		//pos.x = ((cLightPosScreen.x + 1.0f) / 2.0f);
		//pos.y = ((-cLightPosScreen.y + 1.0f) / 2.0f);
		float depth = Sample2D(DepthBuffer, iScreenPos);
		float2 deltaTex = (iScreenPos - pos.xy) / cLSSampleCount;
		float3 color = Sample2D(DiffMap,iScreenPos).xyz;
		float illuminationDecay = 1.0f;
		for (int i = 0; i < cLSSampleCount; i++)
		{
			iScreenPos -= deltaTex;
			float3 s = Sample2D(DiffMap, iScreenPos).xyz;
			s *= illuminationDecay * cLSWeight;
			color += s;
			illuminationDecay *= cLSDecay;
		}
		oColor = float4(color * cLSExposure,depth);
    #endif
    #ifdef COMBINE
		float3 original = Sample2D(DiffMap, iScreenPos) * 0.9f;
		float3 shaft = Sample2D(EmissiveMap, iScreenPos) * 0.6f;
		original *= saturate(1.0 - shaft);
		oColor = float4(original + shaft, 1.0);
    #endif
}
