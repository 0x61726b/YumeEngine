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
#include "deferred.hlsl"

Texture2D PointLightAtt : register(t6);

TextureCube EnvMap : register(t14);

cbuffer LightParameters : register(b3)
{
  float4 LightColor;
  float4 LightPosition;
  float4 LightDirection;

  bool showDiffuse;
  bool showNormals;
  bool showSpec;
  bool showDepth;
}

#ifdef POINTLIGHT_STANDARD
struct PS_INPUT_POS
{
  float4 Position : SV_POSITION;
  float3 ViewRay  : VIEWRAY;
};
#elif POINTLIGHT_BRDF
struct PS_INPUT_POS
{
  float4 Position : SV_POSITION;
  float3 ViewRay  : VIEWRAY;
};
#elif DIRECTIONALLIGHT
struct PS_INPUT_POS
{
  float4 Position : SV_POSITION;
  float2 TexCoord : TEXCOORD0;
  float3 ViewRay  : VIEWRAY;
};
#endif

#include "Lighting.hlsl"

// Toksvig AA for specular highlights
float toksvig_ft(in float3 Na, in float roughness)
{
    float a = roughness*roughness;
    float s = 2.0f / (a * a) - 2.0f;

    float len = length(Na);
    return len/lerp(s, 1, len);
}

#ifdef DIRECTIONALLIGHT

float4 ps_df_pbr(in PS_INPUT_POS inp) : SV_Target
{
  gbuffer gb = unpack_gbuffer(inp.TexCoord);

  float3 N = normalize(gb.normal.xyz * 2.0 - 1.0);

  float3 V = -normalize(inp.ViewRay.xyz);

  if (dot(gb.normal, gb.normal) == 0)
        return gb.diffuse_albedo * float4(main_light.color.rgb, 1.0);

  if(gb.shading_mode == 10)
    return gb.diffuse_albedo;

  if(showDiffuse)
    return gb.diffuse_albedo;

  if(showNormals)
    return float4(N,1);
  if(showDepth)
    return gb.depth / 1000;
  if(showSpec)
    return gb.specular_albedo;

  float3 P = camera_pos + (-V * gb.depth);

  float3 Ll = -LightDirection.xyz;
  float3 L = Ll;

  float NoL = saturate(dot(N, L));

  float roughness = gb.specular_albedo.a;

  const float3 lcolor = float3(1,1,1);

  float power = 1;
  float3 Li = M_PI * (power * main_light.color.rgb) / pow(length(Ll),2);

  float3 f = brdf(L, V, N, gb.diffuse_albedo.rgb, gb.specular_albedo.rgb, roughness);

  float3 T1 = f * Li * NoL;

  float3 toEye = camera_pos - P;

  toEye /= length(toEye);



  float3 T0 = 0;

  return float4(max(T0 + T1,0.0f),1.0f);
}


#endif
float4 ps_df(in PS_INPUT_POS inp) : SV_Target
{
  float4 screenPos = inp.Position;
	float3 viewRay = inp.ViewRay;

	int3 texCoord = int3(screenPos.xy, 0);

	float3 diffuseAlbedo = rt_colors.Load(texCoord).xyz;
	float3 normal = rt_normals.Load(texCoord).xyz;
	float4 spec = rt_specular.Load(texCoord);
	float depth = rt_lineardepth.Load(texCoord).x;

	float3 Na = normal.xyz * 2.0 - 1.0;
	float3 N = normalize(Na);

  float3 View = -normalize(viewRay.xyz);
  float3 position = camera_pos + (-View * depth);
	normal = N;
  #ifdef DIRECTIONAL_LIGHT
	return float4(GetDirectionalLight(diffuseAlbedo, spec, View, depth, normal), 1.0f);
#endif




	float3 specularAlbedo = float3(0.6f,0.6f,0.6f);
  float specPower = 16;

  //Lighting
  float3 L = 0;
  float att = 1.0f;
  float lightRange = LightDirection.w;

#ifdef POINTLIGHT
  L = LightPosition.xyz - position;

  float dist = length( L );

  att = max(0, 1.0f - ( dist / lightRange ) );

  L /= dist;
#elif DIRECTIONALLIGHT
  L = -LightDirection.xyz;
#endif

#ifdef DIRECTIONALLIGHT
  const float DiffuseNormalizationFactor = 1.0f / 3.14159265f;
	float nDotL = saturate( dot( normal, L ) );
	float3 diffuse = nDotL * LightColor.xyz * diffuseAlbedo * DiffuseNormalizationFactor;

  float3 V = camera_pos - position;
	float3 H = normalize( L + V );
	float specNormalizationFactor = ( ( specPower + 8.0f ) / ( 8.0f * 3.14159265f ) );
	float3 specular = pow( saturate( dot( normal, H ) ), specPower ) * specNormalizationFactor * LightColor.xyz * specularAlbedo.xyz * nDotL;

  float t = toksvig_ft(Na,spec.w);

  return float4(( diffuse + specular ) * att,1.0f);
  #elif POINTLIGHT_BRDF
  float3 light = BRDFPointLight(diffuseAlbedo,normal,position,LightColor.xyz,LightPosition.xyz,LightDirection.w,32);

  return float4(light,1.0f);
  #elif POINTLIGHT_STANDARD
  float3 lightDir;
  float diff = GetPointLightDiffuse(normal,position,lightDir);
  float gspec = GetSpecular(normal,-position,lightDir,255);
  return diff * float4(LightColor.xyz * (diffuseAlbedo.rgb + gspec),1.0f);
  #endif
}
