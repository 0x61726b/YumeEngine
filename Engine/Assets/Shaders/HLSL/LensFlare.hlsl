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

cbuffer LensFlareParams : register(b6)
{
  float4 Scale;
  float4 Bias;
}

float4 ps_lensflare_threshold(in PS_INPUT inp) : SV_TARGET
{
  float4 result = max(float4(0,0,0,0),rt_output.Sample(StandardFilter,inp.tex_coord) + Bias)*Scale;
  return result;
}
