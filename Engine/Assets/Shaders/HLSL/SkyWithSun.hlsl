#include "Uniforms.hlsl"
#include "Transform.hlsl"
#include "Samplers.hlsl"
#include "ScreenPos.hlsl"


struct VSOutput
{
  float4 PositionCS : SV_Position;
  float3 TexCoord : TEXCOORD0;
  float3 Bias : BIAS;
};

#ifdef COMPILEVS

cbuffer PSConstants : register(b6)
{
    float3 cSunDirection : packoffset(c0);
}

#endif

#ifdef COMPILEPS
cbuffer PSConstants : register(b6)
{
    float3 cSunDirection;
}
#endif

#ifdef COMPILEGS

cbuffer GSConstants : register(b6)
{
	uint cRTIndex;
}

struct GSOutput
{
	VSOutput VertexShaderOutput;

	uint RTIndex 		: SV_RenderTargetArrayIndex;
	uint VPIndex 		: SV_ViewportArrayIndex;
};

#endif


#ifdef COMPILEVS
VSOutput VS(float4 iPos : POSITION)
{
    VSOutput output;
    float4x3 modelMatrix = iModelMatrix;
    float3 worldPos = GetWorldPos(modelMatrix);
    output.PositionCS =  GetClipPos(worldPos);
    output.TexCoord = float3(iPos.x,iPos.y,iPos.z);
    output.Bias = float3(1,1,1);
    return output;
}
#endif

#ifdef COMPILEGS

[maxvertexcount(3)]
void GS(triangle VSOutput Input[3], inout TriangleStream<GSOutput> TriStream)
{
	[unroll]
	for(uint i = 0; i < 3; ++i)
	{
		GSOutput output;
		output.VertexShaderOutput = Input[i];
		output.RTIndex = cRTIndex;
		output.VPIndex = cRTIndex;

		TriStream.Append(output);
	}

	TriStream.RestartStrip();
}

#endif


//-------------------------------------------------------------------------------------------------
// Calculates the angle between two vectors
//-------------------------------------------------------------------------------------------------
float AngleBetween(in float3 dir0, in float3 dir1)
{
	return acos(dot(dir0, dir1));
}

//-------------------------------------------------------------------------------------------------
// Uses the CIE Clear Sky model to compute a color for a pixel, given a direction + sun direction
//-------------------------------------------------------------------------------------------------
float3 CIEClearSky(in float3 dir, in float3 sunDir)
{
	float gamma = AngleBetween(dir, sunDir);
	float S = AngleBetween(sunDir, float3(0, 1, 0));
	float theta = AngleBetween(dir, float3(0, 1, 0));

	float cosTheta = cos(theta);
	float cosS = cos(S);
	float cosGamma = cos(gamma);

	float num = (0.91f + 10 * exp(-3 * gamma) + 0.45 * cosGamma * cosGamma) * (1 - exp(-0.32f / cosTheta));
	float denom = (0.91f + 10 * exp(-3 * S) + 0.45 * cosS * cosS) * (1 - exp(-0.32f));

	float lum = num / denom;

	// Clear Sky model only calculates luminance, so we'll pick a strong blue color for the sky
	const float3 SkyColor = float3(0.2f, 0.5f, 1.0f);
	const float3 SunColor = float3(1.0f, 0.8f, 0.3f) * 150;
	const float SunWidth = 0.05f;

	float3 color = SkyColor;
	color = lerp(SunColor, SkyColor, saturate(abs(gamma) / SunWidth));

	return max(color * lum, 0);
}

void PS(VSOutput input,
    out float4 oColor : OUTCOLOR0)
{
	float3 dir = normalize(input.TexCoord);

	oColor = float4(CIEClearSky(dir, normalize(cSunDirection)) * float3(1, 1, 1), 1.0f);
}
