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


VS_MESH_OUTPUT MeshVs(in VS_MESH_INPUT input)
{
	VS_MESH_OUTPUT output;

    float4 pos_world = mul(world, float4(input.pos, 1.0));

    output.pos = mul(vp, pos_world);

    // TODO: needs normal matrix!
    output.norm = normalize(mul(world, float4(input.norm, 0.0)));

    // TODO: needs transform!
	output.tangent = input.tangent;
    output.texcoord = input.texcoord;

    return output;
}
