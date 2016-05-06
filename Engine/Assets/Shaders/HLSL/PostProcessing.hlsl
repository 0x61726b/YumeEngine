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

struct PS_INPUT
{
	float4 position	                : SV_POSITION;
	float2 tex_coord                : TEXCOORD0;
	float3 view_ray	                : TEXCOORD1;
};



SamplerState StandardFilter	        : register(s0);
SamplerState ShadowFilter	        : register(s1);

Texture2D rt_output                : register(t0);
Texture2D rt_colors                 : register(t1);
Texture2D rt_specular               : register(t2);
Texture2D rt_normals         	    : register(t3);
Texture2D rt_lineardepth     	    : register(t4);

Texture2D rt_blurred							: register(t8);
Texture2D rt_main_full							: register(t9);
