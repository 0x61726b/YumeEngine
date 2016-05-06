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
  float  g_NumSteps;
  float  g_NumDir;
  float  g_R;
  float  g_inv_R;
  float  g_sqr_R;
  float  g_AngleBias;
  float  g_TanAngleBias;
  float  g_Attenuation;
  float  g_Contrast;
  float  g_AspectRatio;
  float  g_InvAspectRatio;
  float2 g_FocalLen;
  float2 g_InvFocalLen;
  float2 g_InvResolution;
  float2 g_Resolution;
  float2 g_ZLinParams;
  float QualityMode;
  bool debug_ao;
}

cbuffer camera_ps                   : register(b1)
{
    float4x4 vp						: packoffset(c0);
    float4x4 vp_inv					: packoffset(c4);
    float3 camera_pos				: packoffset(c8);
    float z_far						: packoffset(c8.w);
}

Texture2D RandomVec : register(t5);


//----------------------------------------------------------------------------------
float tangent(float3 P, float3 S)
{
    return (P.z - S.z) / length(S.xy - P.xy);
}

//----------------------------------------------------------------------------------
float3 uv_to_eye(float2 uv, float eye_z)
{
    uv = (uv * float2(2.0, -2.0) - float2(1.0, -1.0));
    return float3(uv * g_InvFocalLen * eye_z, eye_z);
}

//----------------------------------------------------------------------------------
float3 fetch_eye_pos(float2 uv)
{
    float z = rt_lineardepth.SampleLevel(StandardFilter,float3(uv,0),0).r;
    return uv_to_eye(uv, z);
}

//----------------------------------------------------------------------------------
float3 tangent_eye_pos(float2 uv, float4 tangentPlane)
{
    // view vector going through the surface point at uv
    float3 V = fetch_eye_pos(uv);
    float NdotV = dot(tangentPlane.xyz, V);
    // intersect with tangent plane except for silhouette edges
    if (NdotV < 0.0) V *= (tangentPlane.w / NdotV);
    return V;
}

float length2(float3 v) { return dot(v, v); }

//----------------------------------------------------------------------------------
float3 min_diff(float3 P, float3 Pr, float3 Pl)
{
    float3 V1 = Pr - P;
    float3 V2 = P - Pl;
    return (length2(V1) < length2(V2)) ? V1 : V2;
}

//----------------------------------------------------------------------------------
float falloff(float r)
{
    return 1.0f - g_Attenuation*r*r;
}

//----------------------------------------------------------------------------------
float2 snap_uv_offset(float2 uv)
{
    return round(uv * g_Resolution) * g_InvResolution;
}

float2 snap_uv_coord(float2 uv)
{
    return uv - (frac(uv * g_Resolution) - 0.5f) * g_InvResolution;
}

//----------------------------------------------------------------------------------
float tan_to_sin(float x)
{
    return x * rsqrt(x*x + 1.0f);
}

//----------------------------------------------------------------------------------
float3 tangent_vector(float2 deltaUV, float3 dPdu, float3 dPdv)
{
    return deltaUV.x * dPdu + deltaUV.y * dPdv;
}

//----------------------------------------------------------------------------------
float invlength(float2 v)
{
    return rsqrt(dot(v,v));
}

//----------------------------------------------------------------------------------
float tangent(float3 T)
{
    return -T.z * invlength(T.xy);
}

//----------------------------------------------------------------------------------
float biased_tangent(float3 T)
{
    float phi = atan(tangent(T)) + g_AngleBias;
    return tan(min(phi, M_PI*0.5));
}

void integrate_direction(inout float ao, float3 P, float2 uv, float2 deltaUV,
                         float numSteps, float tanH, float sinH)
{
    for (float j = 1; j <= numSteps; ++j) {
        uv += deltaUV;
        float3 S = fetch_eye_pos(uv);

        // Ignore any samples outside the radius of influence
        float d2  = length2(S - P);
        if (d2 < g_sqr_R) {
            float tanS = tangent(P, S);

            [branch]
            if(tanS > tanH) {
                // Accumulate AO between the horizon and the sample
                float sinS = tanS / sqrt(1.0f + tanS*tanS);
                float r = sqrt(d2) * g_inv_R;
                ao += falloff(r) * (sinS - sinH);

                // Update the current horizon angle
                tanH = tanS;
                sinH = sinS;
            }
        }
    }
}

//----------------------------------------------------------------------------------
float2 rotate_direction(float2 Dir, float2 CosSin)
{
    return float2(Dir.x*CosSin.x - Dir.y*CosSin.y,
                  Dir.x*CosSin.y + Dir.y*CosSin.x);
}


//----------------------------------------------------------------------------------
float AccumulatedHorizonOcclusion_LowQuality(float2 deltaUV,
                                             float2 uv0,
                                             float3 P,
                                             float numSteps,
                                             float randstep)
{
    // Randomize starting point within the first sample distance
    float2 uv = uv0 + snap_uv_offset( randstep * deltaUV );

    // Snap increments to pixels to avoid disparities between xy
    // and z sample locations and sample along a line
    deltaUV = snap_uv_offset( deltaUV );

    // Add an epsilon in case g_AngleBias==0.0f
    float tanT = tan(-M_PI*0.5 + g_AngleBias + 1.e-5f);
    float sinT = (g_AngleBias != 0.0) ? tan_to_sin(tanT) : -1.0;

    float ao = 0;
    integrate_direction(ao, P, uv, deltaUV, numSteps, tanT, sinT);

    // Integrate opposite directions together
    deltaUV = -deltaUV;
    uv = uv0 + snap_uv_offset( randstep * deltaUV );
    integrate_direction(ao, P, uv, deltaUV, numSteps, tanT, sinT);

    // Divide by 2 because we have integrated 2 directions together
    // Subtract 1 and clamp to remove the part below the surface
    return max(ao * 0.5 - 1.0, 0.0);
}

//----------------------------------------------------------------------------------
float AccumulatedHorizonOcclusion(float2 deltaUV,
                                  float2 uv0,
                                  float3 P,
                                  float numSteps,
                                  float randstep,
                                  float3 dPdu,
                                  float3 dPdv )
{
    // Randomize starting point within the first sample distance
    float2 uv = uv0 + snap_uv_offset( randstep * deltaUV );

    // Snap increments to pixels to avoid disparities between xy
    // and z sample locations and sample along a line
    deltaUV = snap_uv_offset( deltaUV );

    // Compute tangent vector using the tangent plane
    float3 T = deltaUV.x * dPdu + deltaUV.y * dPdv;

    float tanH = biased_tangent(T);
    float sinH = tanH / sqrt(1.0f + tanH*tanH);

    float ao = 0;
    for(float j = 1; j <= numSteps; ++j) {
        uv += deltaUV;
        float3 S = fetch_eye_pos(uv);

        // Ignore any samples outside the radius of influence
        float d2  = length2(S - P);
        if (d2 < g_sqr_R) {
            float tanS = tangent(P, S);

            [branch]
            if(tanS > tanH) {
                // Accumulate AO between the horizon and the sample
                float sinS = tanS / sqrt(1.0f + tanS*tanS);
                float r = sqrt(d2) * g_inv_R;
                ao += falloff(r) * (sinS - sinH);

                // Update the current horizon angle
                tanH = tanS;
                sinH = sinS;
            }
        }
    }

    return ao;
}

//----------------------------------------------------------------------------------
float AccumulatedHorizonOcclusion_Quality(float2 deltaUV,
                                          float2 uv0,
                                          float3 P,
                                          float numSteps,
                                          float randstep,
                                          float3 dPdu,
                                          float3 dPdv )
{
    // Jitter starting point within the first sample distance
    float2 uv = (uv0 + deltaUV) + randstep * deltaUV;

    // Snap first sample uv and initialize horizon tangent
    float2 snapped_duv = snap_uv_offset(uv - uv0);
    float3 T = tangent_vector(snapped_duv, dPdu, dPdv);
    float tanH = tangent(T) + g_TanAngleBias;

    float ao = 0;
    float h0 = 0;
    for(float j = 0; j < numSteps; ++j) {
        float2 snapped_uv = snap_uv_coord(uv);
        float3 S = fetch_eye_pos(snapped_uv);
        uv += deltaUV;

        // Ignore any samples outside the radius of influence
        float d2 = length2(S - P);
        if (d2 < g_sqr_R) {
            float tanS = tangent(P, S);

            [branch]
            if (tanS > tanH) {
                // Compute tangent vector associated with snapped_uv
                float2 snapped_duv = snapped_uv - uv0;
                float3 T = tangent_vector(snapped_duv, dPdu, dPdv);
                float tanT = tangent(T) + g_TanAngleBias;

                // Compute AO between tangent T and sample S
                float sinS = tan_to_sin(tanS);
                float sinT = tan_to_sin(tanT);
                float r = sqrt(d2) * g_inv_R;
                float h = sinS - sinT;
                ao += falloff(r) * (h - h0);
                h0 = h;

                // Update the current horizon angle
                tanH = tanS;
            }
        }
    }
    return ao;
}

float4 ps_ssao(in PS_INPUT input) : SV_Target
{
      float2 tc = input.tex_coord;
      float3 P = fetch_eye_pos(tc);

      // Project the radius of influence g_R from eye space to texture space.
      // The scaling by 0.5 is to go from [-1,1] to [0,1].
      float2 step_size = 0.5 * g_R  * g_FocalLen / P.z;

      // Early out if the projected radius is smaller than 1 pixel.
      float numSteps = min ( g_NumSteps, min(step_size.x * g_Resolution.x, step_size.y * g_Resolution.y));
      if( numSteps < 1.0 ) return 1.0;
      step_size = step_size / ( numSteps + 1 );

      // Nearest neighbor pixels on the tangent plane
      float3 Pr, Pl, Pt, Pb;
      float4 tangentPlane;

      Pr = fetch_eye_pos(input.tex_coord + float2(g_InvResolution.x, 0));
  	  Pl = fetch_eye_pos(input.tex_coord + float2(-g_InvResolution.x, 0));
  	  Pt = fetch_eye_pos(input.tex_coord + float2(0, g_InvResolution.y));
  	  Pb = fetch_eye_pos(input.tex_coord + float2(0, -g_InvResolution.y));
  	  float3 N = normalize(cross(Pr - Pl, Pt - Pb));
  	  tangentPlane = float4(N, dot(P, N));

      // Screen-aligned basis for the tangent plane
      float3 dPdu = min_diff(P, Pr, Pl);
      float3 dPdv = min_diff(P, Pt, Pb) * (g_Resolution.y * g_InvResolution.x);

      // (cos(alpha),sin(alpha),jitter)
      float3 rand = RandomVec.Load(int3((int)input.position.x&63, (int)input.position.y&63, 0)).xyz;

      float ao = 0;
      float d;
      float alpha = 2.0f * M_PI / g_NumDir;



      // this switch gets unrolled by the HLSL compiler
      switch (QualityMode)
      {
      case 0:
          for (d = 0; d < g_NumDir*0.5; ++d) {
              float angle = alpha * d;
              float2 dir = float2(cos(angle), sin(angle));
              float2 deltaUV = rotate_direction(dir, rand.xy) * step_size.xy;
              ao += AccumulatedHorizonOcclusion_LowQuality(deltaUV, input.tex_coord, P, numSteps, rand.z);
          }
          ao *= 2.0;
          break;
      case 1:
          for (d = 0; d < g_NumDir; d++) {
              float angle = alpha * d;
              float2 dir = float2(cos(angle), sin(angle));
              float2 deltaUV = rotate_direction(dir, rand.xy) * step_size.xy;
              ao += AccumulatedHorizonOcclusion(deltaUV, input.tex_coord, P, numSteps, rand.z, dPdu, dPdv);
          }
          break;
      case 2:
          for (d = 0; d < g_NumDir; d++) {
              float angle = alpha * d;
              float2 dir = float2(cos(angle), sin(angle));
              float2 deltaUV = rotate_direction(dir, rand.xy) * step_size.xy;
              ao += AccumulatedHorizonOcclusion_Quality(deltaUV, input.tex_coord, P, numSteps, rand.z, dPdu, dPdv);
          }
          break;
      }


      return 1.0 - ao / g_NumDir * g_Contrast;
}

float4 ps_ssao_merge(in PS_INPUT input) : SV_Target
{
  float4 finalBlurInput = rt_output.Sample(StandardFilter,input.tex_coord);
  float occlusion = rt_blurred.Sample(StandardFilter, input.tex_coord);

  if(debug_ao)
    return occlusion;
  return finalBlurInput*occlusion;
}
