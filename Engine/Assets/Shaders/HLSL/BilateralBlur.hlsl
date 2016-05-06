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

cbuffer SsaoParameters : register(b4)
{
  float g_BlurRadius;
  float g_BlurFalloff;
  float g_Sharpness;
  float g_EdgeThreshold;
  float2 g_InvResolution;
  float2 g_Resolution;
  float2 g_ZLinParams;
}



//-------------------------------------------------------------------------
float fetch_eye_z(float2 uv)
{
    float z = rt_lineardepth.Sample(StandardFilter, float3(uv, 0)).r;
    return z;
}

//-------------------------------------------------------------------------
float BlurFunction(float2 uv, float r, float center_c, float center_d, inout float w_total)
{
    float c = rt_output.Sample( StandardFilter, uv );
    float d = fetch_eye_z(uv);

    float ddiff = d - center_d;
    float w = exp(-r*r*g_BlurFalloff - ddiff*ddiff*g_Sharpness);
    w_total += w;

    return w*c;
}

//-------------------------------------------------------------------------
float4 ps_bilateral_blur_x( in PS_INPUT input ): SV_Target
{
    float b = 0;
    float w_total = 0;
    float center_c = rt_output.Sample( StandardFilter, input.tex_coord );
    float center_d = fetch_eye_z(input.tex_coord);

    for (float r = -g_BlurRadius; r <= g_BlurRadius; ++r)
    {
        float2 uv = input.tex_coord.xy + float2(r*g_InvResolution.x , 0);
        b += BlurFunction(uv, r, center_c, center_d, w_total);
    }

    return b/w_total;
}

float4 ps_bilateral_blur_y( in PS_INPUT input ): SV_Target
{
    float b = 0;
    float w_total = 0;
    float center_c = rt_output.Sample( StandardFilter, input.tex_coord );
    float center_d = fetch_eye_z(input.tex_coord);

    for (float r = -g_BlurRadius; r <= g_BlurRadius; ++r)
    {
        float2 uv = input.tex_coord.xy + float2(0, r*g_InvResolution.y);
        b += BlurFunction(uv, r, center_c, center_d, w_total);
    }
    return b/w_total;
}
