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

SamplerState StandardFilter     : register(s0);

struct VS_MESH_INPUT
{
    float3 pos                  : POSITION;
    float3 norm                 : NORMAL;
	float2 texcoord 	        : TEXCOORD0;
	float3 tangent		        : TANGENT;
};

struct VS_MESH_OUTPUT
{
    float4 pos			        : SV_POSITION;
	float4 norm			        : NORMAL;
	float3 tangent		        : TANGENT;
    float2 texcoord             : TEXCOORD0;
    float3 ldepth               : LINEARDEPTH;
};

struct PS_MESH_OUTPUT
{
	float4 color		        : SV_Target0;
	float4 normal		        : SV_Target1;
	float2 ldepth		        : SV_Target2;
	float4 specular		        : SV_Target3;
};

cbuffer camera_vs               : register(b0)
{
    float4x4 vp                 : packoffset(c0);
    float4x4 vp_inv             : packoffset(c4);
    float3 camera_pos           : packoffset(c8);
    float pad                   : packoffset(c8.w);
}

cbuffer meshdata_vs             : register(b1)
{
    float4x4 world              : packoffset(c0);
}

cbuffer MaterialPs             : register(b0)
{
	  float4 DiffuseColor        : packoffset(c0);
    float4 SpecularColor       : packoffset(c1);
	  float4 EmissiveColor	    : packoffset(c2);
	  bool has_diffuse_tex        : packoffset(c3.x);
	  bool has_normal_tex         : packoffset(c3.y);
	  bool has_specular_tex       : packoffset(c3.z);
	  bool has_alpha_tex          : packoffset(c3.w);
    float ShadingMode           : packoffset(c4.x);
    float Roughness             : packoffset(c4.y);
}

Texture2D diffuse_tex           : register(t0);
Texture2D normal_tex            : register(t1);
Texture2D specular_tex          : register(t2);
Texture2D alpha_tex             : register(t3);
Texture2D roughness_tex             : register(t4);

VS_MESH_OUTPUT MeshVs(in VS_MESH_INPUT input)
{
	VS_MESH_OUTPUT output;

    float4 pos_world = mul(world, float4(input.pos, 1.0));

    output.ldepth = pos_world.xyz - camera_pos;

    output.pos = mul(vp, pos_world);

    // TODO: needs normal matrix!
    output.norm = normalize(mul(world, float4(input.norm, 0.0)));

    // TODO: needs transform!
	output.tangent = input.tangent;
    output.texcoord = input.texcoord;

    return output;
}

// Toksvig AA for specular highlights
float toksvig_ft(in float3 Na, in float roughness)
{
    float a = roughness*roughness;
    float s = 2.0f / (a * a) - 2.0f;

    float len = length(Na);
    return len/lerp(s, 1, len);
}

PS_MESH_OUTPUT MeshPs(in VS_MESH_OUTPUT input)
{
	PS_MESH_OUTPUT output;

	float z = length(input.ldepth);
  //
	// alpha
	if (has_alpha_tex)
	{
		float masked = alpha_tex.Sample(StandardFilter, input.texcoord).x;

		if (masked == 0)
			discard;
	}

	// diffuse
	if (has_diffuse_tex)
		output.color = pow(abs(diffuse_tex.Sample(StandardFilter, input.texcoord)), GAMMA);
	else
        output.color = DiffuseColor;

    // emissive
    if (length(EmissiveColor.rgb) > 0.0)
    {
        if (has_diffuse_tex)
            output.color *= EmissiveColor;
        else
            output.color += EmissiveColor;
    }

	// normals
	if (has_normal_tex)
	{
		float3 tnorm;

		// normal texture
		tnorm = normal_tex.Sample(StandardFilter, input.texcoord).xyz;

		// half to full from texture
		tnorm = tnorm * 2.0 - 1.0;

		float3 n = input.norm.xyz;
		float3 t = input.tangent;
		float3 b = cross(t, n);

		float3 obj_space;
        obj_space.x = dot(tnorm, float3(t.x, b.x, n.x));
        obj_space.y = dot(tnorm, float3(t.y, b.y, n.y));
        obj_space.z = dot(tnorm, float3(t.z, b.z, n.z));

		// back again
        output.normal = float4(obj_space, 1) / 2.0 + 0.5;
	}
	else
		output.normal = float4(normalize(input.norm.xyz)/2.0 + 0.5, 1.0);

    output.normal.a = ShadingMode / 2.0;

    // specular
	if (has_specular_tex)
		output.specular.rgb = specular_tex.Sample(StandardFilter, input.texcoord).rrr;
	else
		output.specular.rgb = SpecularColor.rgb;

    output.specular.a = Roughness;

    // linear depth + z^2 moment for vsm
	output.ldepth = float2(z, z*z);

	return output;
}
