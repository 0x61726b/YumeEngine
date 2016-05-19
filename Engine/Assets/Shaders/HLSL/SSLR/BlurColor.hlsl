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


static const float weights[7] = {0.001f, 0.028f, 0.233f, 0.474f, 0.233f, 0.028f, 0.001f};


float4 ps_sslr_blur_v(in PS_INPUT inp) : SV_TARGET
{
  const int2 offsets[7] = {{0, -3}, {0, -2}, {0, -1}, {0, 0}, {0, 1}, {0, 2}, {0, 3}};
  
  float2 uvs = inp.tex_coord * float2(1600,900); // make sure to send in the SRV's dimensions for cb_depthBufferSize
  // sample level zero since only one mip level is available with the bound SRV
  int3 loadPos = int3(uvs, 0);

  float4 color = float4(0.0f, 0.0f, 0.0f, 1.0f);
  [unroll]
  for(uint i = 0u; i < 7u; ++i)
  {
    color += rt_colors.Load(loadPos, offsets[i]) * weights[i];
  }
  return float4(color.rgb, 1.0f);
}

float4 ps_sslr_blur_h(in PS_INPUT inp) : SV_TARGET
{
  const int2 offsets[7] = {{-3, 0}, {-2, 0}, {-1, 0}, {0, 0}, {1, 0}, {2, 0}, {3, 0}};

  float2 uvs = inp.tex_coord * float2(1600,900); // make sure to send in the SRV's dimensions for cb_depthBufferSize
  // sample level zero since only one mip level is available with the bound SRV
  int3 loadPos = int3(uvs, 0);

  float4 color = float4(0.0f, 0.0f, 0.0f, 1.0f);
  [unroll]
  for(uint i = 0u; i < 7u; ++i)
  {
    color += rt_colors.Load(loadPos, offsets[i]) * weights[i];
  }
  return float4(color.rgb, 1.0f);
}
