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
#include "PostProcessing.hlsl"

cbuffer SsaoParameters : register(b4)
{
  float ssao_scale : packoffset(c0);
}

float ssao(in PS_INPUT inp)
{
    float2 tc = inp.tex_coord;
    float depth  = rt_lineardepth.Sample(StandardFilter, tc).r;
	float3 pos = camera_pos + (normalize(inp.view_ray.xyz) * depth);
    float3 normal = rt_normals.Sample(StandardFilter, tc).rgb * 2.0 - 1.0;

	if (ssao_scale == 0.0)
		return 1.0;

	float w, h;
	rt_lineardepth.GetDimensions(w, h);

	const float2 scale = float2(1.0/w, 1.0/h);

	float ret = 0.0;

	float2 samples[24] = {
		float2(1,1),
		float2(-1,1),
		float2(1,-1),
		float2(-1,-1),
		float2(1,0),
		float2(-1,0),
		float2(0,-1),
		float2(0,-1),

        float2(2, 1),
        float2(-2, 1),
        float2(2, -1),
        float2(-2, -1),
        float2(2, 0),
        float2(-2, 0),
        float2(0, -2),
        float2(0, -2),

        float2(1, 2),
        float2(-1, 2),
        float2(1, -2),
        float2(-1, -2),

        float2(2, 2),
        float2(-2, 2),
        float2(2, -2),
        float2(-2, -2),
	};

	[unroll]
    for (int i = 0; i < 24; ++i)
    {
		// TODO: Needs distance scaling
		float2 ntc = tc + samples[i] * scale * 2 * abs(simple_noise(tc));

		float4 occvr = to_ray(ntc, vp_inv);

		float occdepth = rt_lineardepth.Sample(StandardFilter, ntc.xy).r;

        // bilateral filter
        if (abs(occdepth - depth) > 0.1)
            continue;

        float3 occnorm = rt_normals.Sample(StandardFilter, ntc.xy).xyz * 2.0 - 1.0;
		float3 occpos = camera_pos + (normalize(occvr).xyz * occdepth);

		float3 diff = occpos - pos;
		float3 v = normalize(diff);
		float ddiff = length(diff);

        if (dot(occnorm, normal) < 0.95)
            ret += ssao_scale * saturate(dot(normal, v)) * (1.0 / (1.0 + ddiff));
    }

    return 1.0 - saturate(ret/SSAO_SAMPLES);
}

float4 ps_ssao(in PS_INPUT input) : SV_Target
{
  return ssao(input);
}
