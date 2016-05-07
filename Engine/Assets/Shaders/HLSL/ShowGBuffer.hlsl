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

cbuffer camera_ps                   : register(b1)
{
    float4x4 vp						: packoffset(c0);
    float4x4 vp_inv					: packoffset(c4);
    float3 camera_pos				: packoffset(c8);
    float z_far						: packoffset(c8.w);
}

float4 ps_show_gbuffer( in PS_INPUT input ): SV_Target
{
	float2 screenPos = input.tex_coord;

	//Top left
	if (screenPos.x < 0.5f && screenPos.y < 0.5f)
		return rt_colors.Sample(StandardFilter, input.tex_coord * 2);
	////Top right
	if (screenPos.x > 0.5 && screenPos.y < 0.5)
		return rt_normals.Sample(StandardFilter, float2(screenPos.x * 2 - 1, screenPos.y * 2));
	////Bottom left
	if (screenPos.x < 0.5 && screenPos.y > 0.5)
	{
		float depth = rt_lineardepth.Sample(StandardFilter, float2(screenPos.x * 2, screenPos.y * 2 - 1)).r;
    depth /= z_far;
		return float4(depth, depth, depth, depth);
	}
	////Bottom right
	if (screenPos.x > 0.5 && screenPos.y > 0.5)
		return rt_specular.Sample(StandardFilter, screenPos * 2 - 1);

	return float4(0, 1, 0, 0);
}
