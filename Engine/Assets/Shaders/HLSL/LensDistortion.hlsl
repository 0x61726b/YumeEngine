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


float4 ps_lens(in PS_INPUT inp) : SV_TARGET
{
  float4 color = rt_output.Sample(StandardFilter,inp.tex_coord);

  float2 tex = inp.tex_coord;

  float k = -0.15;

  float kcube = 0.15;

  float r2 = (tex.x-0.5) * (tex.x-0.5) + (tex.y-0.5) * (tex.y-0.5);
  float f = 0;

  if( kcube == 0.0)
  {
      f = 1 + r2 * k;
  }
  else
  {
      f = 1 + r2 * (k + kcube * sqrt(r2));
  }

  float x = f*(tex.x-0.5)+0.5;
  float y = f*(tex.y-0.5)+0.5;

  return float4(rt_output.Sample(StandardFilter,float2(x,y)).rgb,1);
}
