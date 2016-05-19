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
#include "forward.hlsl"

struct VS_MESH_OUTPUT
{
    float4 pos			        : SV_POSITION;
	float4 norm			        : NORMAL;
	float3 tangent		        : TANGENT;
    float2 texcoord             : TEXCOORD0;
};


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
    float FloorRoughness      : packoffset(c4.z);
}

Texture2D diffuse_tex           : register(t0);
Texture2D normal_tex            : register(t1);
Texture2D specular_tex          : register(t2);
Texture2D alpha_tex             : register(t3);
Texture2D roughness_tex             : register(t4);

float4 MeshPs(in VS_MESH_OUTPUT input) : SV_TARGET
{
  float3 N = input.norm.xyz;
  float3 P = input.pos.xyz;
  float3 toEye = camera_pos_ps - P;
  toEye /= length(toEye);



  N = normalize(N);

  float4 color = float4(0,0,0,0);
  if(has_diffuse_tex)
  {
    color = diffuse_tex.Sample(StandardFilter,input.texcoord);
  }
  else
    color = DiffuseColor;

  if(has_normal_tex)
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
    N = obj_space;
  }
  else
    N = input.norm.xyz;


  float3 Ll = -float3(-0.577350300f,-0.577350300f,-0.577350300f).xyz;
  float3 L = Ll;

  float NoL = saturate(dot(N, L));
  float3 V = reflect(L,N);

  float spec = pow(max(dot(V, P), 0.0f), 64);

  const float3 lcolor = float3(1,1,1);

  float power = 1;
  float3 Li = M_PI * (power * lcolor) / pow(length(Ll),2);

  float3 f = brdf(L, V, N, color.rgb,float3(0.7f,0.7f,0.7f), 1);

  float3 T1 = f * Li * NoL;

  return float4(max(T1,0.0f),1.0f);
}
