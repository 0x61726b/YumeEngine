#include "Uniforms.hlsl"
#include "Transform.hlsl"
#include "Samplers.hlsl"
#include "ScreenPos.hlsl"


struct VSOutput
{
  float4 PositionCS : SV_Position;
  float2 TexCoord : TEXCOORD0;
  float3 Bias : BIAS;
};

#ifdef COMPILEVS

cbuffer PSConstants : register(b6)
{
    float3 SunDirection : packoffset(c0);
	uint EnableSun : packoffset(c1);
}

#endif

#ifdef COMPILEPS
cbuffer PSConstants : register(b6)
{
    float3 SunDirection;
}
#endif

#ifdef COMPILEGS

cbuffer GSConstants : register(b6)
{
	uint RTIndex;
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
    output.TexCoord = GetQuadTexCoord(output.PositionCS);
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
		output.RTIndex = RTIndex;
		output.VPIndex = RTIndex;

		TriStream.Append(output);
	}

	TriStream.RestartStrip();
}

#endif

void PS(float2 iTexCoord : TEXCOORD0,
    float2 iScreenPos : TEXCOORD1,
    out float4 oColor : OUTCOLOR0)
{
  oColor = float4(1,1,1,1);
}
