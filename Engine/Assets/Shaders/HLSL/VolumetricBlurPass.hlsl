#include "Uniforms.hlsl"
#include "Transform.hlsl"
#include "Samplers.hlsl"
#include "ScreenPos.hlsl"

static const float2 PixelSize = float2(1.0f / 1600.0f, 1.0f / 900.0f); //RT width / RT Height
static const float Scale = 4;
static const float Spread = 0.005;
static const float Decay = 0.5f;
static const float4 ShaftTint = float4(1,1,1,1);
static const float Blend = 0.5f;

void VS(float4 iPos : POSITION,
	out float4 oTexCoord : TEXCOORD0,
	out float2 oScreenPos : TEXCOORD1,
	out float4 oPos : OUTPOSITION)
{
    static const float2 HalfPixel = float2(1.0f / 1600.0f, 1.0f / 900.0f) * 0.5f;
    float4x3 modelMatrix = iModelMatrix;
    float3 worldPos = GetWorldPos(modelMatrix);
    oPos = GetClipPos(worldPos);
    float2 quad = GetQuadTexCoord(oPos);
    oTexCoord.xy = quad + HalfPixel;
    oTexCoord.zw = quad*2 - 1;
    oScreenPos = GetScreenPosPreDiv(oPos);
}
#define SAMPLE_COUNT 40
#define INV_SAMPLE_COUNT 0.025f

void PS(float4 iTexCoord : TEXCOORD0,
        float2 iScreenPos : TEXCOORD1,
    out float4 oColor : OUTCOLOR0)
{
  #ifdef GODRAY_PASS
  float4 pos = cLightPosScreen;
	pos.x = ((cLightPosScreen.x + 1.0f) / 2.0f);
	pos.y = ((-cLightPosScreen.y + 1.0f) / 2.0f);
	const float NUM_SAMPLES = 24;
	const float Density = 0.4f;
	const float Weight = 0.9f;
	const float Decay = 0.9f;
	const float Exposure = 0.4f;
	float2 deltaTex = (iTexCoord.xy - pos.xy) / NUM_SAMPLES;
	float4 color = Sample2D(DiffMap, iTexCoord.xy);
	float illuminationDecay = 1.0f;
	for (int i = 0; i < NUM_SAMPLES; i++)
	{
		iTexCoord.xy -= deltaTex;
		float4 s = Sample2D(DiffMap, iTexCoord.xy);
		s *= illuminationDecay * Weight;
		color += s;
		illuminationDecay *= Decay;
	}
	oColor = float4((color * Exposure).rgb, color.a);
#endif
#ifdef COMBINE
	half4 shaft = Sample2D(EmissiveMap, iTexCoord.xy);
	shaft.rgb *= ShaftTint;

	half3 LinearColor = Sample2D(DiffMap, iTexCoord.xy).rgb;
	//the mask is stored in alplha
	float fadeShaft = shaft.a * Blend;

	oColor = float4(LinearColor + shaft.rgb * fadeShaft, 1);
#endif
}
