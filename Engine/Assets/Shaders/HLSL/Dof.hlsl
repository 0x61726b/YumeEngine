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
#include "Blur.hlsl"

cbuffer DoFParams : register(b6)
{
  float dof_focal_plane;
  float dof_coc_scale;
}

float4 ps_gauss_dof_h(in PS_INPUT inp) : SV_TARGET
{
    return bilateral_gauss_far(inp, float2(0, 1), dof_coc_scale);
}

float4 ps_gauss_dof_v(in PS_INPUT inp) : SV_TARGET
{
    return bilateral_gauss_far(inp, float2(1, 0), dof_coc_scale);
}

float blur_factor(in float depth)
{
	return smoothstep(0, dof_focal_plane, abs(depth));
}

float4 ps_depth_of_field(in PS_INPUT inp) : SV_Target
{
    float4 blurred = rt_pp_one.Sample(StandardFilter, inp.tex_coord);
    float4 full = rt_output.Sample(StandardFilter, inp.tex_coord);
    float depth = rt_lineardepth.Sample(StandardFilter, inp.tex_coord).x;

    // TODO: a should be 0 or 1, but gets interpolated in between somewhere
    if (full.a < 0.5)
        return full;

    return lerp(full, blurred, saturate(blur_factor(depth)));
}
