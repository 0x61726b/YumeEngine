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

cbuffer LightParameters : register(b6)
{
  float4 LightColor;
  float4 LightPosition;
}

cbuffer ObjectVS : register(b1)
{
  float4x4 volume_transform;
}

cbuffer camera_vs : register(b0)
{
  float4x4 vp;
  float4x4 wv;
  float3 camera_pos;
  float pad;
}



struct PS_INPUT_DF
{
  float4 position : SV_POSITION;
  float4 screen_pos : TEXCOORD0;
  float4 far_ray : TEXCOORD1;
};

struct PS_INPUT_POS
{
  float4 Position : SV_POSITION;
  float3 ViewRay  : VIEWRAY;
};


PS_INPUT_POS vs_df(in float3 position : POSITION)
{
  PS_INPUT_POS output;

  float4 worldPos = mul(volume_transform,float4(position,1.0f));
  output.Position = mul(vp,worldPos);

  float3 pos_view = mul(float4(position,1.0f),wv).xyz;
  output.ViewRay = worldPos - camera_pos;
  return output;
}
