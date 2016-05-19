//----------------------------------------------------------------------------
//Yume Engine
//Copyright (C) 2015  arkenthera
//This program is free software; you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation; either version 2 of the License, or
//(at your option) any later version.
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//You should have received a copy of the GNU General Public License along
//with this program; if not, write to the Free Software Foundation, Inc.,
//51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.*/
//----------------------------------------------------------------------------
//
// File : <Filename>
// Date : <Date>
// Comments :
//
//----------------------------------------------------------------------------

struct VsInput
{
  float3 Position : POSITION;
};

struct VSOutput
{
  float4 Position : SV_Position;
  float3 TexCoord : TEXCOORD0;
  float3 Bias : BIAS;
};

cbuffer camera_vs               : register(b0)
{
    float4x4 vp                 : packoffset(c0);
    float4x4 wvp             : packoffset(c4);
    float3 camera_pos           : packoffset(c8);
    float pad                   : packoffset(c8.w);
}

cbuffer meshdata_vs             : register(b1)
{
    float4x4 world              : packoffset(c0);
}

cbuffer PSConstants : register(b6)
{
    float3 SunDirection : packoffset(c0);
}

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

[maxvertexcount(3)]
void sky_gs(triangle VSOutput Input[3], inout TriangleStream<GSOutput> TriStream)
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


VSOutput sky_vs(VsInput input)
{
  VSOutput output;

  float4 pos_world = mul(world, float4(input.Position,1.0f));
  output.Position = mul(wvp, input.Position).xyww;
  output.TexCoord = float3(input.Position.x,input.Position.y,input.Position.z);
  output.Bias = float3(1,1,1);

  return output;
}

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

float4 sky_ps(VSOutput input) : SV_Target
{
  float3 dir = normalize(input.TexCoord);
  return float4(CIEClearSky(dir, normalize(SunDirection)) * input.Bias, 1.0f);
}
