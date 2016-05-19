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

#include "common.h"
#include "tools.hlsl"
#include "brdf.hlsl"

SamplerState StandardFilter			: register(s0);
SamplerState ShadowFilter			: register(s1);

cbuffer light_ps                    : register(b2)
{
	directional_light main_light	: packoffset(c0);
	float4x4 light_mvp				: packoffset(c3);
    float4x4 light_vp_inv			: packoffset(c7);
    float4x4 light_vp_tex			: packoffset(c11);
}

cbuffer onetime_ps                  : register(b6)
{
	float4 scene_dim_max			: packoffset(c0);
	float4 scene_dim_min			: packoffset(c1);
}

cbuffer camera_ps                   : register(b1)
{
    float4x4 vp_ps						: packoffset(c0);
    float4x4 vp_inv					: packoffset(c4);
		float4x4 ProjMatrix			: packoffset(c8);
    float3 camera_pos_ps				: packoffset(c12);
    float z_far						: packoffset(c12.w);
}

float shadow_attenuation(in float3 pos, in float3 Ll, in Texture2D linear_shadowmap, in float min_s = 0.0, in float min_o = 0.0)
{
    float4 tct = mul(light_vp_tex, float4(pos, 1));
    float2 tc = tct.xy/tct.w;

    if (tc.x >= 1 || tc.y >= 1 || tc.y < 0 || tc.x < 0)
        return min_o;

    float z = length(Ll) * (1.0 - SHADOW_BIAS);

    return min(1.0, vsm(tc, z, linear_shadowmap, ShadowFilter) + min_s);
}

float get_spotlight(in float3 L, in float3 N)
{
    float3 from_light = -L;
    float3 spot_dir = normalize(main_light.normal.xyz);

    float falloff = 0.7;

    float alpha = dot(from_light, spot_dir);

    float attenuation_dist = 1.0;

    if (alpha < falloff)
        attenuation_dist = 1.0 - 10*abs(alpha - falloff);

    return sdot(N, -from_light) * saturate(attenuation_dist);
}
