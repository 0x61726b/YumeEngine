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
#include "tonemapping.hlsl"

cbuffer BloomParameters : register(b4)
{
  float exposure_key;
  float exposure_speed;

  float bloom_sigma;
  float bloom_treshold;
}

float average_luminance()
{
    return rt_pp_one.Load(uint3(0,0,0)).x;
}

// Determines the color based on exposure settings
float3 calc_exposed_color(in float3 color, in float average_lum, in float threshold, out float exposure)
{
	// Use geometric mean
	average_lum = max(average_lum, 0.001f);
	float keyValue = exposure_key;
	float linear_exposure = (exposure_key / average_lum);
	exposure = log2(max(linear_exposure, 0.0001f));
    exposure -= threshold;
    return exp2(exposure) * color;
}

float3 tone_map(in float4 color, in float average_lum, in float threshold, out float exposure)
{
    color.rgb = calc_exposed_color(color.rgb, average_lum, threshold, exposure);
    color.rgb = tonemap_uc2(color.rgb);

    return color.rgb;
}




float ps_adapt_exposure(in PS_INPUT inp) : SV_Target
{
    //get medium brightness of scene
	float curr_lum = luminance(rt_output.SampleLevel(StandardFilter, float2(0.5, 0.5), 10).rgb);
    float last_lum = rt_pp_one.Load(uint3(0, 0, 0)).x;

    float v = last_lum + (curr_lum - last_lum) * (1 - exp(-time_delta * exposure_speed));
    return 0.5f;
}

float4 ps_bloom_treshold(in PS_INPUT inp) : SV_TARGET
{
    float4 color = rt_output.Sample(StandardFilter, inp.tex_coord);

    float avg_lum = average_luminance();
    float exposure = 0;
    color.rgb = calc_exposed_color(color.rgb, avg_lum, bloom_treshold, exposure);

    if (dot(color.rgb, 0.333f) <= 0.01f)
        color = 0.f;

    return float4(color.rgb, 1);
}

float4 ps_bloom(in PS_INPUT inp) : SV_TARGET
{
    float4 color = rt_output.SampleLevel(StandardFilter, inp.tex_coord, 0);
    float avg_lum = average_luminance();

    float exposure = 0;
    color.rgb = tone_map(color.rgba, avg_lum, 0, exposure);

    float4 bloom = rt_pp_two.Sample(ShadowFilter, inp.tex_coord);

    return float4((color + bloom).rgb, color.a);
}

float4 gauss(in PS_INPUT input, in float2 tex_scale, in float sigma)
{
    float4 color = 0;

	float w,h;
	rt_output.GetDimensions(w,h);

    for (int i = -6; i < 6; i++)
    {
		float weight = gauss_weight(i, sigma);

        // compute tap tc
        float2 tc = input.tex_coord;
		tc += (i / float2(w,h)) * tex_scale;

        // fetch tap
		float4 tap = rt_output.Sample(ShadowFilter, tc);
		color += tap * weight;
    }

    return color;
}

float4 ps_gauss_bloom_h(in PS_INPUT inp) : SV_TARGET
{
    return gauss(inp, float2(0, 1), bloom_sigma);
}

float4 ps_gauss_bloom_v(in PS_INPUT inp) : SV_TARGET
{
    return gauss(inp, float2(1, 0), bloom_sigma);
}
