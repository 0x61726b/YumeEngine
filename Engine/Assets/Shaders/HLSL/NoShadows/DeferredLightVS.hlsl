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

cbuffer ObjectVS
{
  float4x4 world;
}

cbuffer camera_vs
{
  float4x4 vp;
  float4x4 vp_inv;
  float4x4 camera_rot;
  float3 camera_pos;
  float pad;
  float4 GBufferOffsets;
  float3 FrustumSize;
}

float4 GetScreenPos(float4 clipPos)
{
    return float4(
        clipPos.x * GBufferOffsets.z + GBufferOffsets.x * clipPos.w,
        -clipPos.y * GBufferOffsets.w + GBufferOffsets.y * clipPos.w,
        0.0,
        clipPos.w);
}

float4 GetFarRay(float4 clipPos)
{
    float4 viewRay = float4(
        clipPos.x / clipPos.w * FrustumSize.x,
        clipPos.y / clipPos.w * FrustumSize.y,
        FrustumSize.z,1.0f);

    return mul(viewRay, camera_rot);
}



struct PS_INPUT_DF
{
  float4 position : SV_POSITION;
  float4 screen_pos : TEXCOORD0;
  float4 far_ray : TEXCOORD1;
};


PS_INPUT_DF vs_df(in float3 position : POSITION)
{
  PS_INPUT_DF inp;

  float4 pos_world = mul(world, float4(position, 1.0));
  inp.position = mul(pos_world, vp);
  inp.screen_pos = GetScreenPos(inp.position);
  inp.far_ray = GetFarRay(inp.position);

  return inp;
}
